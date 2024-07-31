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
						float outputSaturation = 1.0f;

						asioCore->CreateBuffers(selectedInputs, _countof(selectedInputs), selectedOutputs, _countof(selectedOutputs), AsioCore::UsePreferredSize, outputSaturation);

						IProcessingChainPtr processingChain = asioCore->ProcessingChain;

						processingChain->OutputChannelPair[0]->IsActive = true;
						processingChain->InputChannel[0]->IsActive = true;

						asioCore->Start();

						Sleep(30000);

						asioCore->Stop();
					}

					static IController* CreateDebugController(ITransportPtr& transport)
					{
						return new DebugController(transport);
					}

					TEST_METHOD(LooperControl)
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

						ILooperPtr looper = AsioObjectFactory::CreateLooper(config);
						DebugController* debugController = dynamic_cast<DebugController*>(looper->Controller.GetInterfacePtr());

						DWORD looperThreadId;
						HANDLE looperThread = CreateThread(NULL, 0, LooperThread, looper.GetInterfacePtr(), 0, &looperThreadId);
						int loopLength = 2000;

						if (looperThread != NULL)
						{
							try
							{
								// run looper
								Assert::AreEqual(RecordingStatusType::Off, looper->RecordingStatus);
								debugController->PutNext(ControllerCode::Run);
								Assert::AreEqual(RecordingStatusType::Off, looper->RecordingStatus);

								// start loop recording after 1s
								Sleep(1000);
								debugController->PutNext(ControllerCode::Record);

								// stop loop recording to set loop length
								Sleep(loopLength);
								Assert::AreEqual(RecordingStatusType::Recording, looper->RecordingStatus);
								debugController->PutNext(ControllerCode::Record);

								// start overdubbing after half a loop
								Sleep(loopLength >> 1);
								debugController->PutNext(ControllerCode::Record);

								// short delay to process controller
								Sleep(2 * config->ControlResolution);
								Assert::AreEqual(RecordingStatusType::Armed, looper->RecordingStatus);

								// stop recording short before end of 3rd loop
								Sleep(3 * loopLength - 5 * config->ControlResolution);
								Assert::AreEqual(RecordingStatusType::Recording, looper->RecordingStatus);
								debugController->PutNext(ControllerCode::Record);

								// short delay to process controller
								Sleep(2 * config->ControlResolution);
								Assert::AreEqual(RecordingStatusType::Unarmed, looper->RecordingStatus);

								// wait until loop wrapped around
								Sleep(loopLength);
								Assert::AreEqual(RecordingStatusType::Off, looper->RecordingStatus);

								// stop the looper
								debugController->PutNext(ControllerCode::Stop);
								Assert::AreEqual(WAIT_OBJECT_0, WaitForSingleObject(looperThread, loopLength));

								CloseHandle(looperThread);
							}
							catch (...)
							{
								looper->Stop();
								WaitForSingleObject(looperThread, INFINITE);
								CloseHandle(looperThread);
								throw;
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

					static DWORD LooperThread(LPVOID param)
					{
						ILooperPtr looper = static_cast<ILooper*>(param);
						looper->Run();

						return 0;
					}
				};
			}
		}
	}
}