#include "pch.h"
#include "CppUnitTest.h"
#include "HelperMethods.h"
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
				TEST_CLASS(ContainerReaderTests)
				{
				public:

					TEST_METHOD(InMemoryTransfer)
					{
						// create a test container and write it repeatedly to a dynamic container
						ISampleContainerPtr pBufferContainer = HelperMethods::CreateTestContainer(Constants::SampleCount, Constants::ChannelCount);
						ISampleProcessorPtr pWriter = ObjectFactory::CreateToContainerProcessor(Constants::ChannelCount, Constants::SampleCount * 10, Constants::SampleCount * 2);

						int loopsForTenSeconds = (Constants::SampleRate * 10 / Constants::SampleCount) + 1;
						for (int i = 0; i < loopsForTenSeconds; i++)
						{
							pWriter->Process(pBufferContainer);
						}

						// get the resulting full-data container
						ISampleContainerPtr pFullDataContainer = nullptr;
						pWriter->QueryInterface<ISampleContainer>(&pFullDataContainer);
						Assert::IsNotNull(pFullDataContainer.GetInterfacePtr(), L"Can access full data container from vector writer.");
						
						// create a source processor from the full-data container 
						ISampleSourcePtr pSampleSource = ObjectFactory::CreateContainerSource(pFullDataContainer);
						ISampleProcessorPtr pSourceProcessor = ObjectFactory::CreateFromSourceProcessor(pSampleSource);

						// create a new target container writer
						ISampleProcessorPtr pOutputWriter = ObjectFactory::CreateToContainerProcessor(Constants::ChannelCount, Constants::SampleCount * 3, Constants::SampleCount * 1);

						for (int i = 0; i < loopsForTenSeconds; i++)
						{
							pSourceProcessor->Process(pBufferContainer);
							pOutputWriter->Process(pBufferContainer);
						}

						ISampleContainerPtr pResult = nullptr;
						pOutputWriter->QueryInterface<ISampleContainer>(&pResult);
						Assert::IsNotNull(pResult.GetInterfacePtr(), L"Can access output data container from vector writer.");
					}

				};
			}
		}
	}
}