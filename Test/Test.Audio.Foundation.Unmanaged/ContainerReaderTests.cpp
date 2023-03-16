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
						ISampleContainerPtr pBufferContainer = HelperMethods::CreateTestContainer(Constants::SampleCount, Constants::ChannelCount);
						ISampleProcessorPtr pWriter = ObjectFactory::CreateToContainerProcessor(Constants::ChannelCount, Constants::SampleCount * 10, Constants::SampleCount * 2);

						int loopsForTenSeconds = (Constants::SampleRate * 10 / Constants::SampleCount) + 1;
						for (int i = 0; i < loopsForTenSeconds; i++)
						{
							pWriter->Process(pBufferContainer);
						}

						ISampleContainerPtr pFullDataContainer = nullptr;
						pWriter->QueryInterface<ISampleContainer>(&pFullDataContainer);
						Assert::IsNotNull(pFullDataContainer.GetInterfacePtr(), L"Can access full data container from vector writer.");
						
						ISampleSourcePtr pSampleSource = ObjectFactory::CreateContainerSource(pFullDataContainer, Constants::SampleCount);
						ISampleProcessorPtr pOutputWriter = ObjectFactory::CreateToContainerProcessor(Constants::ChannelCount, Constants::SampleCount * 3, Constants::SampleCount * 1);

						pSampleSource->First = pOutputWriter;

						bool readSecondHalf = false;

						for (int i = 0; i < loopsForTenSeconds; i++)
						{
							pSampleSource->OnNextBuffer(readSecondHalf);
							readSecondHalf = !readSecondHalf;
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