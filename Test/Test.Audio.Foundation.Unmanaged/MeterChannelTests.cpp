#include "pch.h"
#include "CppUnitTest.h"
#include "Constants.h"
#include <ISampleBuffer.h>
#include <ObjectFactory.h>
#include <MemCheck.h>

using namespace Test::Audio::Common;
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

						ISampleContainerPtr pResultBuffers = ObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);
						ISampleProcessorPtr pTarget = ObjectFactory::CreateToContainerProcessor(pResultBuffers);
						

						ISampleProcessorPtr pMeterProcessor = nullptr;
						Assert::AreEqual(S_OK, pMeterChannel->QueryInterface(&pMeterProcessor), L"Can get ISampleProcessor from IMeterChannel");

						ProcessingContext context(Constants::SampleCount);

						pMeterProcessor->Process(pTestBuffers, context);
						pTarget->Process(pTestBuffers, context);

						for (int c = 0; c < Constants::ChannelCount; c++)
						{
							for (int s = 0; s < Constants::SampleCount; s++)
							{
								Assert::AreEqual(pTestBuffers->Channels[c]->Samples[s], pResultBuffers->Channels[c]->Samples[s], L"Write-through is lossless");
							}
						}
					}
					TEST_METHOD_INITIALIZE(Init)
					{
						m_memCheck.BeginCheck();
					}

					TEST_METHOD_CLEANUP(CleanUp)
					{
						m_memCheck.EndCheck();
					}
				private:
					MemCheck m_memCheck;

				};
			}
		}
	}
}
