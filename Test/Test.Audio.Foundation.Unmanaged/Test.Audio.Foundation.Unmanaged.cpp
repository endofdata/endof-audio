#include "pch.h"
#include "CppUnitTest.h"
#include <ISampleBuffer.h>
#include <ObjectFactory.h>


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Foundation::Unmanaged;

namespace Test
{
	namespace Audio
	{
		namespace Foundation
		{
			namespace Unmanaged
			{
				class Constants
				{
				public:
					static const int SampleCount = 256;
					static const int ChannelCount = 2;
					static const int SampleRate = 48000;
				};

				class HelperMethods
				{
				public:
					static void TestSampleBuffer(ISampleBufferPtr pBuffer, int sampleCount)
					{
						Assert::AreEqual(sampleCount, pBuffer->SampleCount, L"Buffer sample count is OK");

						for (int s = 0; s < sampleCount; s++)
						{
							float testValue = (float)s / 12.0f;
							pBuffer->Samples[s] = testValue;

							Assert::AreEqual(testValue, pBuffer->Samples[s], L"Buffer read/write retains value");
						}
						Assert::IsNotNull(pBuffer->SamplePtr, L"Buffer pointer is not NULL");
					}

					static void TestSampleContainer(ISampleContainerPtr pContainer, int sampleCount, int channelCount)
					{
						Assert::AreEqual(sampleCount, pContainer->SampleCount, L"Container sample count is OK");
						Assert::AreEqual(channelCount, pContainer->ChannelCount, L"Container channel count is OK");
						Assert::IsFalse(pContainer->IsActive, L"Container is not active initially.");

						for (int c = 0; c < channelCount; c++)
						{
							HelperMethods::TestSampleBuffer(pContainer->Channels[c], sampleCount);
						}
					}
				};

				TEST_CLASS(MeterChannelTests)
				{
				public:
					TEST_METHOD(BasicInitialization)
					{
						IMeterChannelPtr pMeterChannel = ObjectFactory::CreateMeterChannel(Constants::SampleRate, Constants::ChannelCount);

						Assert::AreEqual(Constants::ChannelCount, pMeterChannel->ChannelCount, L"Meter channel count is OK");
					}

					TEST_METHOD(WriteThrough)
					{
						IMeterChannelPtr pMeterChannel = ObjectFactory::CreateMeterChannel(Constants::SampleRate, Constants::ChannelCount);
						ISampleContainerPtr pTestBuffers = ObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						for (int c = 0; c < Constants::ChannelCount; c++)
						{
							for (int s = 0; s < Constants::SampleCount; s++)
							{
								pTestBuffers->Channels[c]->Samples[s] = (float)(1.0f - 2.0 / s);
							}
						}

						ISampleJoinerPtr pWriteThrough = ObjectFactory::CreateSampleJoiner(Constants::SampleCount, Constants::ChannelCount);

						pMeterChannel->WriteThrough = pWriteThrough;

						ISampleReceiverPtr pMeterInput;
						Assert::AreEqual(S_OK, pMeterChannel->QueryInterface(&pMeterInput));

						pMeterInput->Receive(pTestBuffers);

						ISampleContainerPtr pResultBuffers;
						Assert::AreEqual(S_OK, pWriteThrough->QueryInterface(&pResultBuffers));

						Assert::AreEqual(Constants::ChannelCount, pResultBuffers->ChannelCount, L"Write-through channel count OK");
						Assert::AreEqual(Constants::SampleCount, pResultBuffers->SampleCount, L"Write-through channel count OK");

						for (int c = 0; c < Constants::ChannelCount; c++)
						{
							for (int s = 0; s < Constants::SampleCount; s++)
							{
								Assert::AreEqual(pTestBuffers->Channels[c]->Samples[s], pResultBuffers->Channels[c]->Samples[s], L"Write-through is lossless");
							}
						}
					}
				};

				TEST_CLASS(SampleContainerTests)
				{
				public:
					TEST_METHOD(BasicInitialization)
					{
						ISampleContainerPtr pContainer = ObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
					}

					TEST_METHOD(AddAndRemoveChannels)
					{
						ISampleContainerPtr pContainer = ObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						pContainer->ChannelCount += 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount + 2);

						pContainer->ChannelCount -= 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
					}

					TEST_METHOD(ResizeBuffers)
					{
						ISampleContainerPtr pContainer = ObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);

						pContainer->SampleCount *= 2;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount * 2, Constants::ChannelCount);

						pContainer->SampleCount = Constants::SampleCount;

						HelperMethods::TestSampleContainer(pContainer, Constants::SampleCount, Constants::ChannelCount);
					}

				};

				TEST_CLASS(SampleBufferTests)
				{
				public:

					TEST_METHOD(BasicInitialization)
					{
						ISampleBufferPtr pBuffer = ObjectFactory::CreateSampleBuffer(Constants::SampleCount);

						HelperMethods::TestSampleBuffer(pBuffer, Constants::SampleCount);
					}
				};


			}
		}
	}
}
