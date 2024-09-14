#include "pch.h"
#include "CppUnitTest.h"
#include "HelperMethods.h"
#include "Constants.h"
#include <ISampleBuffer.h>
#include <FoundationObjectFactory.h>
#include <ProcessingContext.h>
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
				typedef std::shared_ptr<ProcessingContext> ProcessingContextPtr;

				TEST_CLASS(ContainerReaderTests)
				{
				public:
					TEST_METHOD(InMemoryTransfer)
					{
						ISampleContainerPtr pBufferContainer;
						ISampleProcessorPtr pWriter;

						ProcessingContextPtr pContext = CreateTenSecondLoop(pBufferContainer, pWriter);

						ISampleContainerPtr pResult = CreateResultContainer(pBufferContainer, pWriter, pContext, 0, true);

						Assert::AreEqual(pContext->SamplePosition, pResult->SampleCount, L"result length matches processing context sample position.");
					}

					TEST_METHOD(CropToLastTake)
					{
						ISampleContainerPtr pBufferContainer;
						ISampleProcessorPtr pWriter;

						ProcessingContextPtr pContext = CreateTenSecondLoop(pBufferContainer, pWriter);
						int takeLength = static_cast<int>((static_cast<float>(pContext->SamplePosition) / 4.3));

						ISampleContainerPtr pResult = CreateResultContainer(pBufferContainer, pWriter, pContext, takeLength, false);

						Assert::AreEqual(pContext->SamplePosition, pResult->SampleCount, L"result length matches processing context sample position.");
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
					ProcessingContextPtr CreateTenSecondLoop(ISampleContainerPtr& pBufferContainer, ISampleProcessorPtr& pWriter)
					{
						// create a test container and write it repeatedly to a dynamic container
						pBufferContainer = HelperMethods::CreateTestContainer(
							Constants::SampleCount, Constants::ChannelCount, Constants::Keys, Constants::SampleRate);
						pWriter = FoundationObjectFactory::CreateRecorder(
							Constants::ChannelCount, Constants::SampleCount * 10, Constants::SampleCount * 2);

						ProcessingContextPtr context(new ProcessingContext(Constants::SampleCount));

						int loopsForTenSeconds = (Constants::SampleRate * 10 / Constants::SampleCount) + 1;

						for (int i = 0; i < loopsForTenSeconds; i++)
						{
							int processedSamples = pWriter->Process(pBufferContainer, *context);
							context->SamplePosition += processedSamples;
						}
						return context;
					}

					ISampleContainerPtr CreateResultContainer(ISampleContainerPtr& pBufferContainer, ISampleProcessorPtr& pWriter, 
						ProcessingContextPtr& context, int sampleCount, bool allTakes, int fadeIn = 10, int fadeOut = 10)
					{
						// get the resulting data container
						IRecorderPtr pRecorder = nullptr;
						pWriter->QueryInterface<IRecorder>(&pRecorder);
						Assert::IsNotNull(pRecorder.GetInterfacePtr(), L"Can access recorder from vector writer.");
						
						ISampleContainerPtr pRecordingContainer = pRecorder->CreateSampleContainer(false, sampleCount, fadeIn, fadeOut, allTakes);
						Assert::IsNotNull(pRecordingContainer.GetInterfacePtr(), L"Can create data container from recorder.");

						if (sampleCount != 0)
						{
							Assert::AreEqual(sampleCount, pRecordingContainer->SampleCount, L"result length matches requested take length.");
						}

						// create a source processor from the data container 
						ISampleSourcePtr pSampleSource = FoundationObjectFactory::CreateContainerSource(pRecordingContainer);
						ISampleProcessorPtr pSourceProcessor = FoundationObjectFactory::CreateFromSourceProcessor(pSampleSource, MixParameter());

						// create a new target container writer
						ISampleProcessorPtr pOutputWriter = FoundationObjectFactory::CreateRecorder(
							Constants::ChannelCount, Constants::SampleCount * 3, Constants::SampleCount * 1);

						int loopsForTenSeconds = (Constants::SampleRate * 10 / Constants::SampleCount) + 1;
						for (int i = 0; i < loopsForTenSeconds; i++)
						{
							pSourceProcessor->Process(pBufferContainer, *context);
							pOutputWriter->Process(pBufferContainer, *context);
						}

						IRecorderPtr pOutputRecorder = nullptr;
						pOutputWriter->QueryInterface<IRecorder>(&pOutputRecorder);
						ISampleContainerPtr pResult = pOutputRecorder->CreateSampleContainer(false, 0, 10, 10);
						Assert::IsNotNull(pResult.GetInterfacePtr(), L"Can access output data container from output recorder.");

						return pResult;
					}

					MemCheck m_memCheck;

				};
			}
		}
	}
}