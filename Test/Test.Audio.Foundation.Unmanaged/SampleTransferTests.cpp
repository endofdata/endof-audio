#include "pch.h"
#include "CppUnitTest.h"
#include "HelperMethods.h"
#include "Constants.h"
#include <ISampleBuffer.h>
#include <IInputChannel.h>
#include <IOutputChannelPair.h>
#include <ObjectFactory.h>
#include <SampleType.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <SampleConversionUnmanaged.h>

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
						ITakeSequencePtr pTakeSequence = ObjectFactory::CreateTakeSequence();

						Assert::AreEqual(0, pTakeSequence->get_TakeCount(), L"New takeSequence has no takes");
						int sampleRate = Constants::SampleRate;
						double seconds = 1.0;

						ISampleContainerPtr container = ObjectFactory::CreateSampleContainer((int)(sampleRate * seconds), 2);

						Time position = 0;
						Time length = (Time)(seconds * 1000);

						ITakePtr pTake = ObjectFactory::CreateTake(container, position, length);

						int takeId = pTakeSequence->AddTake(pTake);

						Assert::AreEqual(1, pTakeSequence->get_TakeCount(), L"Take count is updated");
						ITakePtr pTakeFromTakeSequence = pTakeSequence->Takes[0];

						Assert::IsNotNull(pTakeFromTakeSequence.GetInterfacePtr(), L"Can retrieve take");

						Assert::AreEqual(takeId, pTakeFromTakeSequence->Id, L"Take-ID is OK");

						Time movedPosition = (Time)(seconds * 3000);
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

						IInputChannelPtr pInput = hwBuffers.CreateInputChannel(0);

						ISampleSourcePtr pInputSource = nullptr;
						pInput->QueryInterface<ISampleSource>(&pInputSource);
						Assert::IsNotNull(pInputSource.GetInterfacePtr(), L"Can access ISampleSource from IInputChannel");

						IOutputChannelPairPtr pOutputPair = hwBuffers.CreateOutputChannelPair(0, 1);

						ISampleProcessorPtr pOutputPairProcessor = nullptr;
						pOutputPair->QueryInterface<ISampleProcessor>(&pOutputPairProcessor);
						Assert::IsNotNull(pOutputPairProcessor.GetInterfacePtr(), L"Can access ISampleProcessor from IOutputChannelPair");

						pInputSource->First = pOutputPairProcessor;

						bool readSecondHalf = false;
						bool writeSecondHalf = !readSecondHalf;

						pInput->IsActive = true;

						pOutputPair->OnNextBuffer(writeSecondHalf);
						pInputSource->OnNextBuffer(readSecondHalf);

						hwBuffers.AssertBufferTransfers(writeSecondHalf);

						hwBuffers.ResetBuffers();

						readSecondHalf = !readSecondHalf;
						writeSecondHalf = !writeSecondHalf;

						pOutputPair->OnNextBuffer(writeSecondHalf);
						pInputSource->OnNextBuffer(readSecondHalf);

						hwBuffers.AssertBufferTransfers(writeSecondHalf);
					}

					TEST_METHOD(ProcessingChainWithSequence)
					{
						HardwareBuffers hwBuffers(Constants::SampleCount);

						IInputChannelPtr pInput = hwBuffers.CreateInputChannel(0);

						ISampleSourcePtr pInputSource = nullptr;
						pInput->QueryInterface<ISampleSource>(&pInputSource);
						Assert::IsNotNull(pInputSource.GetInterfacePtr(), L"Can access ISampleSource from IInputChannel");

						IOutputChannelPairPtr pOutputPair = hwBuffers.CreateOutputChannelPair(0, 1);

						ISampleProcessorPtr pOutputPairProcessor = nullptr;
						pOutputPair->QueryInterface<ISampleProcessor>(&pOutputPairProcessor);
						Assert::IsNotNull(pOutputPairProcessor.GetInterfacePtr(), L"Can access ISampleProcessor from IOutputChannelPair");

						ITakeSequencePtr pTakeSequence = ObjectFactory::CreateTakeSequence();
						Assert::IsNotNull(pTakeSequence.GetInterfacePtr(), L"Can create take sequence");

						ISampleProcessorPtr pTakeSequenceProcessor = nullptr;
						pTakeSequence->QueryInterface<ISampleProcessor>(&pTakeSequenceProcessor);
						Assert::IsNotNull(pTakeSequenceProcessor.GetInterfacePtr(), L"Can access ISampleProcessor from ITakeSequence");

						pInputSource->First = pTakeSequenceProcessor;
						pTakeSequenceProcessor->Next = pOutputPairProcessor;

						int sampleRate = Constants::SampleRate;
						double seconds = 1.0;
						int channelCount = 2;

						ISampleContainerPtr pContainer = ObjectFactory::CreateSampleContainer((int)(seconds * sampleRate), channelCount);
						Assert::IsNotNull(pContainer.GetInterfacePtr(), L"Can create sample container");

						InitSampleBuffers(pContainer, -0.16, -0.32);

						Time position = 0;
						Time length = (Time)(seconds * 1000);
						ITakePtr pTake = ObjectFactory::CreateTake(pContainer, position, length);
						Assert::IsNotNull(pTake.GetInterfacePtr(), L"Can create audio take");

						int takeId = pTakeSequence->AddTake(pTake);

						bool readSecondHalf = false;
						bool writeSecondHalf = !readSecondHalf;

						pInput->IsActive = true;

						pOutputPair->OnNextBuffer(writeSecondHalf);
						pInputSource->OnNextBuffer(readSecondHalf);

						hwBuffers.AssertBufferTransfers(writeSecondHalf);

						hwBuffers.ResetBuffers();

						readSecondHalf = !readSecondHalf;
						writeSecondHalf = !writeSecondHalf;

						pOutputPair->OnNextBuffer(writeSecondHalf);
						pInputSource->OnNextBuffer(readSecondHalf);

						hwBuffers.AssertBufferTransfers(writeSecondHalf);
					}

				private:
					void InitSampleBuffers(ISampleContainerPtr pContainer, sample valueChn0, sample valueChn1)
					{
						sample* pSamplesChn0 = pContainer->Channels[0]->SamplePtr;
						sample* pSamplesChn1 = pContainer->Channels[1]->SamplePtr;
						int sampleCount = pContainer->SampleCount;

						for (int i = 0; i < sampleCount; i++)
						{
							*pSamplesChn0++ = valueChn0;
							*pSamplesChn1++ = valueChn1;
						}
					}

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
							InitInt32Buffer(_inBufferA, 0.01);
							InitInt32Buffer(_inBufferB, 0.02);
							InitInt32Buffer(_outBufferA1, 0.04);
							InitInt32Buffer(_outBufferB1, 0.08);
							InitInt32Buffer(_outBufferA2, 0.16);
							InitInt32Buffer(_outBufferB2, 0.32);
						}

						IInputChannelPtr CreateInputChannel(int hwChannelId)
						{
							IInputChannelPtr pInput = ObjectFactory::CreateInputChannel(Int32LSB,
								hwChannelId, _inBufferA.get(), _inBufferB.get(), _sampleCount);
							Assert::IsNotNull(pInput.GetInterfacePtr(), L"Can create input channel");

							return pInput;
						}

						IOutputChannelPairPtr CreateOutputChannelPair(int hwChannelId1, int hwChannelId2)
						{
							IOutputChannelPairPtr pOutputPair = ObjectFactory::CreateOutputChannelPair(Int32LSB,
								hwChannelId1, _outBufferA1.get(), _outBufferB1.get(),
								hwChannelId2, _outBufferA2.get(), _outBufferB2.get(), _sampleCount);
							Assert::IsNotNull(pOutputPair.GetInterfacePtr(), L"Can create output channel pair");

							return pOutputPair;
						}

						void AssertBufferTransfers(bool writeSecondHalf)
						{
							int ioTolerance = 2;

							if (writeSecondHalf)
							{
								// Input buffer A copied to output buffers B1 and B2
								AssertBufferEquals(_inBufferA, _outBufferB1, ioTolerance);
								AssertBufferEquals(_inBufferA, _outBufferB2, ioTolerance);
								// Output buffers A1 and A2 are not modified
								AssertBufferValue(_outBufferA1, 0.04);
								AssertBufferValue(_outBufferA2, 0.16);
							}
							else
							{
								// Input buffer B copied to output buffers A1 and A2
								AssertBufferEquals(_inBufferB, _outBufferA1, ioTolerance);
								AssertBufferEquals(_inBufferB, _outBufferA2, ioTolerance);
								// Output buffers B1 and B2 are not modified
								AssertBufferValue(_outBufferB1, 0.08);
								AssertBufferValue(_outBufferB2, 0.32);
							}

							// Input buffers are not modified
							AssertBufferValue(_inBufferA, 0.01);
							AssertBufferValue(_inBufferB, 0.02);
						}

						void AssertBufferEquals(const std::unique_ptr<int>& bufferX, const std::unique_ptr<int>& bufferY, int tolerance = 0)
						{
							int* pX = bufferX.get();
							int* pY = bufferY.get();

							for (int i = 0; i < _sampleCount; i++)
							{
								int sampleX = *pX++;
								int sampleY = *pY++;

								Assert::IsTrue(std::abs(sampleX - sampleY) < tolerance, L"Sample difference between input and output is in accepted range");
							}
						}

						void AssertBufferValue(const std::unique_ptr<int>& buffer, sample value)
						{
							int* p = buffer.get();
							int convertedValue = SampleConversion::SampleToInt32(value);

							for (int i = 0; i < _sampleCount; i++)
							{
								int sample = *p++;

								Assert::AreEqual(sample, convertedValue, L"Sample value is not modified");
							}
						}

					private:
						std::unique_ptr<int> CreateInt32Buffer(int _sampleCount)
						{
							return std::unique_ptr<int>(new int[_sampleCount]);
						}

						void InitInt32Buffer(std::unique_ptr<int>& buffer, sample initialValue = 0.0)
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

				};
			}
		}
	}
}