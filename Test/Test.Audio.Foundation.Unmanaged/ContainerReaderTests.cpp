#include "pch.h"
#include "CppUnitTest.h"
#include "HelperMethods.h"
#include "Constants.h"
#include <ISampleBuffer.h>
#include <ObjectFactory.h>
#include <ProcessingContext.h>

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
				TEST_CLASS(ContainerReaderTests)
				{
				public:

					TEST_METHOD(InMemoryTransfer)
					{
						// create a test container and write it repeatedly to a dynamic container
						ISampleContainerPtr pBufferContainer = HelperMethods::CreateTestContainer(Constants::SampleCount, Constants::ChannelCount);
						ISampleProcessorPtr pWriter = ObjectFactory::CreateRecorder(Constants::ChannelCount, Constants::SampleCount * 10, Constants::SampleCount * 2);

						ProcessingContext context;

						int loopsForTenSeconds = (Constants::SampleRate * 10 / Constants::SampleCount) + 1;
						for (int i = 0; i < loopsForTenSeconds; i++)
						{
							pWriter->Process(pBufferContainer, context);
						}

						// get the resulting full-data container
						IRecorderPtr pRecorder = nullptr;
						pWriter->QueryInterface<IRecorder>(&pRecorder);
						Assert::IsNotNull(pRecorder.GetInterfacePtr(), L"Can access recorder from vector writer.");

						ISampleContainerPtr pFullDataContainer = pRecorder->CreateSampleContainer(false, 10, 10);
						Assert::IsNotNull(pFullDataContainer.GetInterfacePtr(), L"Can create full data container from recorder.");

						// create a source processor from the full-data container 
						ISampleSourcePtr pSampleSource = ObjectFactory::CreateContainerSource(pFullDataContainer);
						ISampleProcessorPtr pSourceProcessor = ObjectFactory::CreateFromSourceProcessor(pSampleSource);

						// create a new target container writer
						ISampleProcessorPtr pOutputWriter = ObjectFactory::CreateRecorder(Constants::ChannelCount, Constants::SampleCount * 3, Constants::SampleCount * 1);

						for (int i = 0; i < loopsForTenSeconds; i++)
						{
							pSourceProcessor->Process(pBufferContainer, context);
							pOutputWriter->Process(pBufferContainer, context);
						}

						IRecorderPtr pOutputRecorder = nullptr;
						pOutputWriter->QueryInterface<IRecorder>(&pOutputRecorder);
						ISampleContainerPtr pResult = pOutputRecorder->CreateSampleContainer(false, 10, 10);
						Assert::IsNotNull(pResult.GetInterfacePtr(), L"Can access output data container from output recorder.");
					}
				};
			}
		}
	}
}