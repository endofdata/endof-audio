#include "pch.h"
#include "CppUnitTest.h"
#include "DebugController.h"
#include "ToStringImpl.h"
#include <MemCheck.h>
#include <AsioCore.h>
#include <SteinbergUrRt2.h>
#include <conio.h>
#include <AsioObjectFactory.h>
#include <FoundationObjectFactory.h>
#define INITGUID
#include <AsioDebugDriverGuid.h>
#undef INITGUID

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Test::Audio::Common;
using namespace Audio::Asio::Unmanaged;

namespace Test
{
	namespace Audio
	{
		namespace Asio
		{
			namespace Unmanaged
			{
				TEST_CLASS(TestAudioAsioUnmanaged)
				{
				public:

					TEST_METHOD(Run30SecondsAsioCoreIO)
					{
						bool isPrioritySet = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

						Assert::IsTrue(isPrioritySet, L"Process priority can be set to HIGH_PRIORITY_CLASS");

						AsioCorePtr asioCore = AsioCore::CreateInstancePtr(IID_STEINBERG_UR_RT2);

						int selectedInputs[] = { 0, 1 };
						int selectedOutputs[] = { 0, 1 };
						int sampleCount = 512;
						float inputSaturation = 0.0f;
						float outputSaturation = 0.0f;

						asioCore->CreateBuffers(selectedInputs, _countof(selectedInputs), selectedOutputs, _countof(selectedOutputs), 
							AsioCore::UsePreferredSize, inputSaturation, outputSaturation);

						IProcessingChainPtr processingChain = asioCore->ProcessingChain;

						processingChain->OutputChannelPair[0]->IsActive = true;
						processingChain->InputChannel[0]->IsActive = true;

						asioCore->Start();

						Sleep(30000);

						asioCore->Stop();
					}

					static IController* CreateDebugController(ITransportPtr& transport, void* param)
					{
						return new DebugController(transport);
					}

					TEST_METHOD(LooperControl)
					{
						RunLooperTest([](ILooperPtr& looper, DebugController& debugController, int loopLength, int controlResolution)
						{
							// start loop recording after 1s
							Sleep(1000);
							debugController.PutNext(ControllerCode::Record);

							// stop loop recording to set loop length
							Sleep(loopLength);
							Assert::AreEqual(RecordingStatusType::Recording, looper->RecordingStatus, L"looper is recording");
							debugController.PutNext(ControllerCode::Record);

							// start overdubbing after half a loop
							Sleep(loopLength >> 1);
							debugController.PutNext(ControllerCode::Record);

							// short delay to process controller
							Sleep(2 * controlResolution);
							Assert::AreEqual(RecordingStatusType::Armed, looper->RecordingStatus, L"looper starts recording at next start of loop");

							// stop recording short before end of 3rd loop
							Sleep(3 * loopLength - 5 * controlResolution);
							Assert::AreEqual(RecordingStatusType::Recording, looper->RecordingStatus, L"looper is recording");
							debugController.PutNext(ControllerCode::Record);

							// short delay to process controller
							Sleep(2 * controlResolution);
							Assert::AreEqual(RecordingStatusType::Unarmed, looper->RecordingStatus, L"looper stops recoding at next end of loop");

							// wait until loop wrapped around
							Sleep(loopLength);
							Assert::AreEqual(RecordingStatusType::Off, looper->RecordingStatus, L"looper stopped recording");
						});
					}

					TEST_METHOD(LooperRestart)
					{
						RunLooperTest([](ILooperPtr& looper, DebugController& debugController, int loopLength, int controlResolution)
						{
							for (int pass = 0; pass < 2; pass++)
							{
								Assert::IsTrue(looper->IsRunning, L"looper is running");
								Assert::IsTrue(debugController.IsActive, L"controller is active");

								// stop looper after 1s
								Sleep(1000);
								debugController.PutNext(ControllerCode::Stop);

								Sleep(2 * controlResolution);

								Assert::AreEqual(false, debugController.IsActive, L"controller is inactive");
								Assert::AreEqual(false, looper->IsRunning, L"looper is stopped");

								if (pass == 0)
								{
									looper->Start();
								}
							}
						});
					}

					

					TEST_METHOD_INITIALIZE(Init)
					{
						m_memCheck.BeginCheck();
					}

					TEST_METHOD_CLEANUP(CleanUp)
					{
						if (m_looper != nullptr)
						{
							m_looper->Stop(INFINITE);
							m_looper = nullptr;
						}
						m_memCheck.EndCheck();
					}
				private:
					MemCheck m_memCheck;
					ILooperPtr m_looper;

					void RunLooperTest(std::function<void(ILooperPtr&, DebugController&, int, int)> looperTest)
					{
						ILooperConfigPtr config = AsioObjectFactory::CreateLooperConfiguration();

						config->AsioDevice = CLSID_AsioDebugDriver;
						config->AddInputChannel(0);
						config->AddOutputChannel(0);
						// instead of selecting a MIDI input, we supply a controller factory, that creates a DebugController
						config->MidiInput = -1;
						config->ControllerFactory = CreateDebugController;
						config->ControlResolution = 100;
						config->SampleCount = 512;

						m_looper = AsioObjectFactory::CreateLooper(config);
						DebugController* debugController = dynamic_cast<DebugController*>(m_looper->Controller.GetInterfacePtr());

						// looperThread invokes ILooper::Run() and blocks until looper is stopped
						m_looper->Start();
						int loopLength = 2000;
						Assert::AreEqual(RecordingStatusType::Off, m_looper->RecordingStatus);

						// run specific test
						looperTest(m_looper, *debugController, loopLength, config->ControlResolution);
					}
				};
			}
		}
	}
}