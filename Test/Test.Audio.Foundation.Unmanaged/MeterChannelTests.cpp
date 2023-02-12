#include "pch.h"
#include "CppUnitTest.h"
#include "Constants.h"
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
			}
		}
	}
}
