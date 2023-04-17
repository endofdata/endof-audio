#include "pch.h"
#include <initguid.h>
#include <AsioDebugDriverGuid.h>

#include <AsioCore.h>
#include <ObjectFactory.h>
#include <AsioCoreException.h>
#include <VstObjectFactory.h>
#include <VstCom.h>

#include "SteinbergUrRt2.h"
#include "TransportControl.h"

using namespace RepeatMyself;
using namespace Audio::Asio;
using namespace Audio::Vst::Unmanaged;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;

void writeContents(ISampleContainerPtr& container)
{
	for (int c = 0; c < container->ChannelCount; c++)
	{
		std::wostringstream builder;
		builder << L"fred_" << c << L".dat" << std::ends;

		auto filename = builder.str();

		std::ofstream out(filename, std::ios_base::out + std::ios_base::binary + std::ios_base::trunc);
		auto data = reinterpret_cast<const char*>(container->Channels[c]->SamplePtr);

		out.write(data, container->SampleCount * sizeof(Sample));
		out.flush();
	}
}

int main()
{
	int selectedInputs[] = { 0 };
	int selectedOutputs[] = { 0, 1 };
	int sampleCount = 512;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (hr != S_OK)
	{
		std::wcerr << L"COM initialization failed." << std::endl;
		exit(-1);
	}

	try
	{
		AsioCorePtr device = nullptr;

		try
		{
			device = AsioCore::CreateInstancePtr(IID_STEINBERG_UR_RT2);
			//device = AsioCore::CreateInstancePtr(CLSID_AsioDebugDriver);

			device->CreateBuffers(selectedInputs, _countof(selectedInputs), selectedOutputs, _countof(selectedOutputs), sampleCount);

			float samplesPerTenSecs = device->SampleRate * 60.0f;

			// create processor chain input -> vst -> recorder -> output
			IVstHostPtr host = VstObjectFactory::CreateVstHost(L"RepeatMyself", device->SampleCount, device->SampleRate);

			// const wchar_t* pwcszLibName = L"C:\\Program Files\\Common Files\\VST3\\Unfiltered Audio Indent.vst3";
			const wchar_t* pwcszLibName = L"D:\\Gamer I5\\Documents\\Projects\\vst3sdk\\out\\build\\x64-Debug\\VST3\\Debug\\adelay.vst3\\Contents\\x86_64-win\\adelay.vst3";
			const wchar_t* pluginIdRaw = host->AddLibrary(pwcszLibName);

			if (pluginIdRaw == nullptr)
			{
				std::wcerr << L"Failed to load plugin '" << pwcszLibName << L"'." << std::endl;
			}
			else
			{
				std::wstring pluginId(pluginIdRaw);
				ISampleProcessorPtr vstProcessor = host->CreateSampleProcessor(pluginId.c_str());
				vstProcessor->IsBypassed = false;

				IRecorderPtr recorder = ObjectFactory::CreateRecorder(_countof(selectedInputs), (int)samplesPerTenSecs, (int)samplesPerTenSecs);
				ISampleProcessorPtr recordingProcessor = nullptr;
				recorder->QueryInterface<ISampleProcessor>(&recordingProcessor);
				recordingProcessor->IsBypassed = true;

				IProcessingChainPtr processingChain = device->ProcessingChain;
				ITransportPtr transport = processingChain->Transport;
				int vstId = processingChain->AddProcessor(vstProcessor);
				int recorderId = processingChain->AddProcessor(recordingProcessor);

				ISourceJoinerPtr joiner = ObjectFactory::CreateSourceJoiner();
				ISampleProcessorPtr joiningProcessor = nullptr;
				joiner->QueryInterface<ISampleProcessor>(&joiningProcessor);
				int joinerId = processingChain->AddProcessor(joiningProcessor);

				processingChain->OutputChannelPair[0]->IsActive = true;

				TransportControlPtr transportControl = TransportControl::Create(transport);

				if (transportControl == nullptr)
				{
					std::wcerr << L"TransportControl initialization failed." << std::endl;
				}
				else
				{
					bool isRecordPressed = false;
					int dubCount = 1;

					// start audio device
					device->Start();

					// activate processing
					transportControl->IsActive = true;

					TransportCode transportStatus = TransportCode::None;

					while (transportStatus != TransportCode::Stop)
					{
						std::wcout << transport->HostClock->CurrentTime.ToString() << '\r';

						// check for control input (MIDI)
						if (transportControl->GetNext(1000, transportStatus))
						{
							switch (transportStatus)
							{
							case TransportCode::Record:
								if (!transport->IsLooping)
								{
									// Initial loop recording
									if (recordingProcessor->IsBypassed)
									{
										// Step 1: press record to start
										processingChain->InputChannel[0]->IsActive = true;
										recordingProcessor->IsBypassed = false;
										transport->Start();
										std::wcout << L"Record" << std::endl;
									}
									else
									{
										// Step 2: press record again to stop recording and begin loop
										AudioTime switchTime = transport->HostClock->CurrentTime;

										ISampleContainerPtr take = recorder->CreateSampleContainer(false);
										ISampleSourcePtr takeSource = ObjectFactory::CreateContainerSource(take);
										takeSource->IsLooping = true;
										joiner->AddSource(takeSource);

										std::wcout << L"Loop length " << switchTime.ToString() << std::endl;
										transport->LoopEnd = switchTime;
										transport->IsLooping = true;
									}
								}
								else
								{
									// Toggle recording start / end with next loop wrap
									isRecordPressed = !isRecordPressed;
									std::wcout << (isRecordPressed ? L"Standby" : L"Rehearse") << std::endl;
								}
								break;
							case TransportCode::Locate:
								// Triggered by master loop wrap: delayed handling of 'Record' commands
								if (isRecordPressed)
								{
									isRecordPressed = false;

									if (recordingProcessor->IsBypassed)
									{
										// Step n: start overdub
										recordingProcessor->IsBypassed = false;
										std::wcout << L"Start overdub " << ++dubCount << std::endl;
									}
									else
									{
										// Step n + 1: stop overdub and add loop
										AudioTime switchTime = transport->HostClock->CurrentTime;

										ISampleContainerPtr take = recorder->CreateSampleContainer(false);
										ISampleSourcePtr takeSource = ObjectFactory::CreateContainerSource(take);
										takeSource->IsLooping = true;
										joiner->AddSource(takeSource);

										std::wcout << L"Accept overdub " << dubCount;
										if (switchTime > transport->LoopEnd)
										{
											std::wcout << L", loop length adjusted to " << switchTime.ToString();
											transport->LoopEnd = switchTime;
										}
										std::wcout << std::endl;
									}
								}
								break;
							case TransportCode::Start:
								// Drop current recording, continue looping
								recorder->DropRecording(false);
								std::wcout << L"Drop overdub " << dubCount << std::endl;
								dubCount--;
								break;

							case TransportCode::Stop:
								transport->Stop();
								if (!recordingProcessor->IsBypassed)
								{
									recorder->DropRecording(false);
									std::wcout << L"Drop overdub " << dubCount << std::endl;
									dubCount--;
								}
								std::wcout << L"Transport stopped." << std::endl;
								break;
							default:
								break;
							}
						}
					}

					std::wcout << L"Shutting down everything. Bye!" << std::endl;

					device->Stop();

					// Ensure that processors can be released before the VST plugin libraries are unloaded
					processingChain->RemoveAllProcessors();
				}
			}
		}
		catch (const AsioCoreException& acx)
		{
			std::wcerr << L"Failed to create device. " << acx.Message << L" Error code: 0x" << std::hex << acx.Error << std::dec << std::endl;
		}
	}
	catch (const std::exception& error)
	{
		std::wcerr << L"Unhandled exception: " << error.what() << std::endl;
	}
	CoUninitialize();
}
