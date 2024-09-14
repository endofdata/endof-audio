#include "pch.h"
#include "CppUnitTest.h"
#include "Constants.h"
#include "HelperMethods.h"
#include <ISampleBuffer.h>
#include <ISourceJoiner.h>
#include <FoundationObjectFactory.h>
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
				TEST_CLASS(SourceJoinerTests)
				{
				public:
					TEST_METHOD(BasicInitialization)
					{
						int sampleCount = 12000;
						int channelCount = 2;
						MixParameter mix;

						ISampleSourcePtr pSource = CreateContainerSource(sampleCount, channelCount);
						ISourceJoinerPtr pJoiner = FoundationObjectFactory::CreateSourceJoiner();

						pJoiner->AddSource(pSource, mix);

						Assert::AreEqual(1, pJoiner->SourceCount, L"can add first source");
						ISampleSourcePtr pSourceAgain = pJoiner->Sources[pSource->Id];
						Assert::IsNotNull(pSourceAgain.GetInterfacePtr(), L"source is accessible by its GUID");

						MixParameter current = pJoiner->Parameter[pSource->Id];

						Assert::AreEqual(mix.Level, current.Level, L"mix level is configured");
						Assert::AreEqual(mix.Pan, current.Pan, L"mix panorama is configured");

						Assert::IsTrue(pJoiner->RemoveSource(pSource->Id), L"can remove first source");
						Assert::AreEqual(0, pJoiner->SourceCount, L"source count drops to zero");

						Assert::ExpectException<std::invalid_argument>([pJoiner, pSource]() { pJoiner->Sources[pSource->Id]; }, 
							L"requesting source after removal throws std::invalid_argument.");
					}
					
					TEST_METHOD(MulitpleSources)
					{
						int sampleCount = 24000;
						int sampleRate = 48000;
						int channelCount = 2;
						double scale = static_cast<double>(sampleRate) / 220.0;
						MixParameter mix;

						mix.Level = 0.25;

						std::function<void(ISampleBufferPtr&, int)> init = [sampleCount, scale](ISampleBufferPtr& buffer, int channelId)
						{
							int bufferSize = buffer->SampleCount;
							int scaleInt = static_cast<int>(scale);

							Sample* pWritePos = buffer->SamplePtr;
							for (int s = 0; s < bufferSize; s++)
							{
								*pWritePos++ = static_cast<Sample>(2.0 * static_cast<double>(s % scaleInt) / scale - 1.0);
							}
						};

						ISourceJoinerPtr pJoiner = FoundationObjectFactory::CreateSourceJoiner();

						pJoiner->AddSource(CreateContainerSource(sampleCount, channelCount, &init), mix);
						pJoiner->AddSource(CreateContainerSource(static_cast<int>(sampleCount * 0.75), channelCount, &init), mix);
						pJoiner->AddSource(CreateContainerSource(static_cast<int>(sampleCount * 1.25), channelCount, &init), mix);

						Assert::AreEqual(3, pJoiner->SourceCount, L"can add three sources");

						ISampleProcessorPtr pProcessor = pJoiner;

						ProcessingContext context(sampleCount);
						ISampleContainerPtr pTarget = FoundationObjectFactory::CreateSampleContainer(context.SampleCount, channelCount);						
						// reminder: raw float sample data can be imported by Audacity
						ISampleProcessorPtr pOutFile = FoundationObjectFactory::CreateToFileProcessor(L"d:\\temp\\test_out.dat");

						for (int rounds = 0; rounds < 50; rounds++)
						{
							int samplesDone = pProcessor->Process(pTarget, context);

							Assert::AreEqual(context.SampleCount, samplesDone, L"processed sample count is controlled by context");
							
							pOutFile->Process(pTarget, context);

							pTarget->Clear();
							for (int i = 0; i < pJoiner->SourceCount; i++)
							{
								ISampleSourcePtr source = pJoiner->GetSourceByIndex(i);
								if (source != nullptr)
								{
									source->SamplePosition = 0;
								}
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

					ISampleSourcePtr CreateContainerSource(int sampleCount, int channelCount, 
						std::function<void(ISampleBufferPtr&, int)>* initChannel = nullptr)
					{
						ISampleContainerPtr pContainer = FoundationObjectFactory::CreateSampleContainer(sampleCount, channelCount);
						
						if (initChannel != nullptr)
						{
							for (int channelId = 0; channelId < channelCount; channelId++)
							{
								ISampleBufferPtr channel = pContainer->Channels[channelId];
								(*initChannel)(channel, channelId);
							}
						}
						return FoundationObjectFactory::CreateContainerSource(pContainer);
					}
				};
			}
		}
	}
}
