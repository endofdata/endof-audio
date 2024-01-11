#include "pch.h"
#include "ToStringImpl.h"
#include "CppUnitTest.h"
#include "HelperMethods.h"
#include "Constants.h"
#include <ISampleBuffer.h>
#include <ISampleSharer.h>
#include <IInputChannel.h>
#include <IOutputChannelPair.h>
#include <FoundationObjectFactory.h>
#include <SampleType.h>
#include <SampleConversionUnmanaged.h>
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
				TEST_CLASS(SampleTransferTests)
				{
				public:

					TEST_METHOD(BasicInitialization)
					{
						IHostClockPtr pHostClock = FoundationObjectFactory::CreateHostClock();
						ITransportPtr pTransport = FoundationObjectFactory::CreateTransport(pHostClock, Constants::SampleCount);
						ITakeSequencePtr pTakeSequence = FoundationObjectFactory::CreateTakeSequence(pTransport);

						Assert::AreEqual(0, pTakeSequence->get_TakeCount(), L"New takeSequence has no takes");
						int sampleRate = Constants::SampleRate;
						double seconds = 1.0;

						ISampleContainerPtr container = FoundationObjectFactory::CreateSampleContainer((int)(sampleRate * seconds), 2);

						AudioTime position = 0;
						AudioTime length = static_cast<long long>(seconds * 1000ul);

						ITakePtr pTake = FoundationObjectFactory::CreateTake(container, position, length);

						int takeId = pTakeSequence->AddTake(pTake);

						Assert::AreEqual(1, pTakeSequence->get_TakeCount(), L"Take count is updated");
						ITakePtr pTakeFromTakeSequence = pTakeSequence->Takes[0];

						Assert::IsNotNull(pTakeFromTakeSequence.GetInterfacePtr(), L"Can retrieve take");

						Assert::AreEqual(takeId, pTakeFromTakeSequence->Id, L"Take-ID is OK");

						AudioTime movedPosition = static_cast<long long>(seconds * 3000ul);
						Assert::IsTrue(pTakeSequence->MoveTake(takeId, movedPosition), L"Can move take by ID");

						ITakePtr pMovedTake = pTakeSequence->FindTake(takeId);
						Assert::AreEqual(takeId, pMovedTake->Id, L"Moved take-ID is OK");

						pTakeSequence->RemoveTake(takeId);
						Assert::AreEqual(0, pTakeSequence->TakeCount, L"Take count updated on remove");

						Assert::IsNotNull(pMovedTake.GetInterfacePtr(), L"Can access moved take even after removing it from takeSequence");
						Assert::AreEqual(movedPosition, pMovedTake->Position, L"Moved position is OK");
					}

					TEST_METHOD(ProcessingChain)
					{
						HardwareBuffers hwBuffers(Constants::SampleCount);
						IProcessingChainPtr pProcessingChain = CreateProcessingChain(hwBuffers);

						bool writeSecondHalf = true;

						pProcessingChain->OnNextBuffer(writeSecondHalf);

						hwBuffers.AssertBufferTransfers(writeSecondHalf);

						hwBuffers.ResetBuffers();

						writeSecondHalf = !writeSecondHalf;

						pProcessingChain->OnNextBuffer(writeSecondHalf);

						hwBuffers.AssertBufferTransfers(writeSecondHalf);
					}

					TEST_METHOD(ProcessingChainWithSequence)
					{
						HardwareBuffers hwBuffers(Constants::SampleCount);
						IProcessingChainPtr pProcessingChain = CreateProcessingChain(hwBuffers);

						// Create take sequence
						IHostClockPtr pHostClock = FoundationObjectFactory::CreateHostClock();
						ITransportPtr pTransport = FoundationObjectFactory::CreateTransport(pHostClock, Constants::SampleCount);
						ITakeSequencePtr pTakeSequence = FoundationObjectFactory::CreateTakeSequence(pTransport);
						Assert::IsNotNull(pTakeSequence.GetInterfacePtr(), L"Can create take sequence");

						// Add take sequence as sample processor
						ISampleProcessorPtr pTakeSequenceProcessor = nullptr;
						pTakeSequence->QueryInterface<ISampleProcessor>(&pTakeSequenceProcessor);
						Assert::IsNotNull(pTakeSequenceProcessor.GetInterfacePtr(), L"Can access ISampleProcessor from ITakeSequence");
						pProcessingChain->AddProcessor(pTakeSequenceProcessor);

						// Create some dummy audio take and add it to sequence
						int sampleRate = Constants::SampleRate;
						double seconds = 1.0;
						int channelCount = 2;

						ISampleContainerPtr pContainer = FoundationObjectFactory::CreateSampleContainer((int)(seconds * sampleRate), channelCount);
						Assert::IsNotNull(pContainer.GetInterfacePtr(), L"Can create sample container");

						InitSampleBuffers(pContainer, -0.16f, -0.32f);

						AudioTime position = 0;
						AudioTime length = AudioTime::FromSeconds(seconds);
						ITakePtr pTake = FoundationObjectFactory::CreateTake(pContainer, position, length);

						Assert::IsNotNull(pTake.GetInterfacePtr(), L"Can create audio take");

						int takeId = pTakeSequence->AddTake(pTake);

						bool writeSecondHalf = true;

						pHostClock->Start();

						pProcessingChain->OnNextBuffer(writeSecondHalf);

						hwBuffers.AssertBufferTransfers(writeSecondHalf, pContainer);

						hwBuffers.ResetBuffers();

						writeSecondHalf = !writeSecondHalf;

						pProcessingChain->OnNextBuffer(writeSecondHalf);

						hwBuffers.AssertBufferTransfers(writeSecondHalf, pContainer);
					}


					TEST_METHOD(ProcessingChainWithVectorWriter)
					{
						HardwareBuffers hwBuffers(Constants::SampleCount);
						IProcessingChainPtr pProcessingChain = CreateProcessingChain(hwBuffers);

						// Create VectorWriter and attach it to input sample source
						int initialSize = Constants::SampleRate * 5;	// buffer for five seconds
						int growth = Constants::SampleRate * 2;			// grow for two seconds
						ISampleProcessorPtr pVectorWriter = FoundationObjectFactory::CreateRecorder(2, initialSize, growth);

						pProcessingChain->AddProcessor(pVectorWriter);
						
						bool writeSecondHalf = true;

						int loopsRequiredForTenSeconds = (Constants::SampleRate * 10 / Constants::SampleCount) + 1;
						for (int loop = 0; loop < loopsRequiredForTenSeconds; loop++)
						{
							pProcessingChain->OnNextBuffer(writeSecondHalf);
							writeSecondHalf = !writeSecondHalf;
						}

						// Get sample container with output from vector writer
						IRecorderPtr pRecorder = nullptr;
						pVectorWriter->QueryInterface<IRecorder>(&pRecorder);
						Assert::IsNotNull(pRecorder.GetInterfacePtr(), L"Can access IRecorder from ISampleProcessor (vector writer)");

						ISampleContainerPtr pOutputContainer = pRecorder->CreateSampleContainer(false, 10, 10);						
						Assert::IsNotNull(pOutputContainer.GetInterfacePtr(), L"Can create ISampleContainer from IRecorder (vector writer)");
						Assert::AreEqual(2, pOutputContainer->ChannelCount, L"Has two channels of output data");
						Assert::AreEqual(loopsRequiredForTenSeconds * Constants::SampleCount, pOutputContainer->SampleCount, L"Has expected size of output data");
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
					class HardwareBuffers
					{
					public:
						HardwareBuffers(int sampleCount)
						{
							_sampleCount = sampleCount;
							_inBufferA = CreateInt32Buffer(_sampleCount);
							_inBufferB = CreateInt32Buffer(_sampleCount);

							_outBufferA1 = CreateInt32Buffer(_sampleCount);
							_outBufferB1 = CreateInt32Buffer(_sampleCount);
							_outBufferA2 = CreateInt32Buffer(_sampleCount);
							_outBufferB2 = CreateInt32Buffer(_sampleCount);

							ResetBuffers();
						}

						void ResetBuffers()
						{
							InitInt32Buffer(_inBufferA, 0.01f);
							InitInt32Buffer(_inBufferB, 0.02f);
							InitInt32Buffer(_outBufferA1, 0.04f);
							InitInt32Buffer(_outBufferB1, 0.08f);
							InitInt32Buffer(_outBufferA2, 0.16f);
							InitInt32Buffer(_outBufferB2, 0.32f);
						}

						IInputChannelPtr CreateInputChannel(int hwChannelId)
						{
							IInputChannelPtr pInput = FoundationObjectFactory::CreateInputChannel(Int32LSB,
								hwChannelId, _inBufferA.get(), _inBufferB.get(), _sampleCount);
							Assert::IsNotNull(pInput.GetInterfacePtr(), L"Can create input channel");

							return pInput;
						}

						IOutputChannelPairPtr CreateOutputChannelPair(int hwChannelId1, int hwChannelId2)
						{
							IOutputChannelPairPtr pOutputPair = FoundationObjectFactory::CreateOutputChannelPair(Int32LSB,
								hwChannelId1, _outBufferA1.get(), _outBufferB1.get(),
								hwChannelId2, _outBufferA2.get(), _outBufferB2.get(), _sampleCount);
							Assert::IsNotNull(pOutputPair.GetInterfacePtr(), L"Can create output channel pair");

							return pOutputPair;
						}

						void AssertBufferTransfers(bool writeSecondHalf, const ISampleContainerPtr& pMixedIn = nullptr)
						{
							int ioTolerance = 16;
							const Sample* pMixedInLeft = nullptr;
							const Sample* pMixedInRight = nullptr;

							if (pMixedIn != nullptr)
							{
								pMixedInLeft = pMixedIn->Channels[0]->SamplePtr;
								pMixedInRight = pMixedIn->Channels[1]->SamplePtr;
							}
							
							if (writeSecondHalf)
							{								
								// input buffer A + pMixedInLeft -> output buffers B1
								AssertBufferEquals(_inBufferA, _outBufferB1, ioTolerance, pMixedInLeft);
								// (input buffer B or cleared) + pMixedInRight -> output buffer B2 
								AssertBufferValue(_outBufferB2, pMixedInRight != nullptr? *pMixedInRight : 0.0f);
								// output buffers A1 and A2 are not modified
								AssertBufferValue(_outBufferA1, 0.04f);
								AssertBufferValue(_outBufferA2, 0.16f);
							}
							else
							{
								// input buffer B + pMixedInLeft -> output buffers A1
								AssertBufferEquals(_inBufferB, _outBufferA1, ioTolerance, pMixedInLeft);
								// (input bufer A or cleared) + pMixedInRight -> output buffer A2
								AssertBufferValue(_outBufferA2, pMixedInRight != nullptr ? *pMixedInRight : 0.0f);
								// Output buffers B1 and B2 are not modified
								AssertBufferValue(_outBufferB1, 0.08f);
								AssertBufferValue(_outBufferB2, 0.32f);
							}

							// Input buffers are not modified
							AssertBufferValue(_inBufferA, 0.01f);
							AssertBufferValue(_inBufferB, 0.02f);
						}

						void AssertBufferEquals(const std::unique_ptr<int>& bufferX, const std::unique_ptr<int>& bufferY, int tolerance = 0, const Sample* pOffset = nullptr)
						{
							int* pX = bufferX.get();
							int* pY = bufferY.get();
							Sample internTolerance = SampleConversion::Int32ToSample(tolerance);

							for (int i = 0; i < _sampleCount; i++)
							{
								int sampleX = *pX++;
								int sampleY = *pY++;
								Sample internX = SampleConversion::Int32ToSample(sampleX);
								Sample internY = SampleConversion::Int32ToSample(sampleY);

								if (pOffset != nullptr)
								{
									internY -= *pOffset;
									sampleY -= SampleConversion::SampleToInt32(*pOffset++);
								}
								AssertSampleInRange(internX, internY, internTolerance);
								AssertSampleInRange(sampleX, sampleY, tolerance);
							}
						}

						void AssertBufferValue(const std::unique_ptr<int>& buffer, Sample value)
						{
							int* p = buffer.get();
							int convertedValue = SampleConversion::SampleToInt32(value);

							for (int i = 0; i < _sampleCount; i++)
							{
								int sample = *p++;

								Assert::AreEqual(sample, convertedValue, L"Sample value is not modified");
							}
						}

						void AssertSampleInRange(Sample x, Sample y, Sample tolerance)
						{
							if (std::abs(x - y) >= tolerance)
							{
								std::wostringstream builder;
								builder << L"Difference between input " << std::setprecision(8) << x << L" and output " << y << L" of internal sample is in accepted range " << tolerance;
								Assert::Fail(builder.str().c_str());
							}
						}

						void AssertSampleInRange(int x, int y, int tolerance)
						{
							if (std::abs(x - y) >= tolerance)
							{
								std::wostringstream builder;
								builder << L"Difference between input " << x << L" and output " << y << L" of external sample is in accepted range " << tolerance;
								Assert::Fail(builder.str().c_str());
							}
						}

					private:
						std::unique_ptr<int> CreateInt32Buffer(int _sampleCount)
						{
							return std::unique_ptr<int>(new int[_sampleCount]);
						}

						void InitInt32Buffer(std::unique_ptr<int>& buffer, Sample initialValue = 0.0)
						{
							int* writePos = buffer.get();

							for (int i = 0; i < _sampleCount; i++)
							{
								*writePos++ = SampleConversion::SampleToInt32(initialValue);
							}
						}

						int _sampleCount;
						std::unique_ptr<int> _inBufferA;
						std::unique_ptr<int> _inBufferB;
						std::unique_ptr<int> _outBufferA1;
						std::unique_ptr<int> _outBufferB1;
						std::unique_ptr<int> _outBufferA2;
						std::unique_ptr<int> _outBufferB2;
					};

					void InitSampleBuffers(ISampleContainerPtr pContainer, Sample valueChn0, Sample valueChn1)
					{
						Sample* pSamplesChn0 = pContainer->Channels[0]->SamplePtr;
						Sample* pSamplesChn1 = pContainer->Channels[1]->SamplePtr;
						int sampleCount = pContainer->SampleCount;

						for (int i = 0; i < sampleCount; i++)
						{
							*pSamplesChn0++ = valueChn0;
							*pSamplesChn1++ = valueChn1;
						}
					}

					IProcessingChainPtr CreateProcessingChain(HardwareBuffers& hwBuffers)
					{
						// Create hardware buffers, input channel, output channel pair
						IInputChannelPtr pInput = hwBuffers.CreateInputChannel(0);
						pInput->Mix.Level = 1.0;
						pInput->Mix.Pan = PanLeft;
						IOutputChannelPairPtr pOutputPair = hwBuffers.CreateOutputChannelPair(0, 1);

						// Create processing chain for the input and output channels
						IHostClockPtr pHostClock = FoundationObjectFactory::CreateHostClock(Constants::SampleRate);
						ITransportPtr pTransport = FoundationObjectFactory::CreateTransport(pHostClock, Constants::SampleCount);
						ISampleContainerPtr pContainer = FoundationObjectFactory::CreateSampleContainer(Constants::SampleCount, Constants::ChannelCount);
						IProcessingChainPtr pProcessingChain = FoundationObjectFactory::CreateProcessingChain(pTransport, pContainer);

						pProcessingChain->AddInput(pInput);
						pProcessingChain->AddOutputPair(pOutputPair);
						pInput->IsActive = true;
						pOutputPair->IsActive = true;

						return pProcessingChain;
					}

					MemCheck m_memCheck;
				};
			}
		}
	}
}