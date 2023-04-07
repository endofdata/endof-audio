#include "pch.h"
#include <initguid.h>
#include <AsioDebugDriverGuid.h>

#include <AsioCore.h>
#include <ObjectFactory.h>
#include <AsioCoreException.h>
#include "SteinbergUrRt2.h"
#include "TransportControl.h"

using namespace RepeatMyself;
using namespace Audio::Asio;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;

void writeContents(ISampleContainerPtr& container)
{
	for (int c = 0; c < container->ChannelCount; c++)
	{
		std::ostrstream builder;
		builder << "fred_" << c << ".dat" << std::ends;

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
		std::cerr << "COM initialization failed." << std::endl;
		exit(-1);
	}

	try
	{
		TransportControlPtr transport = TransportControl::Create();

		if (transport == nullptr)
		{
			std::cerr << "TransportControl initialization failed." << std::endl;
		}
		else
		{
			AsioCorePtr device = nullptr;

			try
			{
				device = AsioCore::CreateInstancePtr(IID_STEINBERG_UR_RT2);
				//device = AsioCore::CreateInstancePtr(CLSID_AsioDebugDriver);
			}
			catch (const AsioCoreException& acx)
			{
				std::cerr << "Failed to create device. " << acx.Message << " Error code: " << std::hex << acx.Error << std::endl;
				exit(-1);
			}

			device->CreateBuffers(selectedInputs, _countof(selectedInputs), selectedOutputs, _countof(selectedOutputs), sampleCount);

			float samplesPerTenSecs = device->SampleRate * 60.0f;

			// create processor chain input -> recorder -> output
			IRecorderPtr recorder = ObjectFactory::CreateRecorder(_countof(selectedInputs), (int)samplesPerTenSecs, (int)samplesPerTenSecs);
			ISampleProcessorPtr recordingProcessor = nullptr;
			recorder->QueryInterface<ISampleProcessor>(&recordingProcessor);
			recordingProcessor->IsBypassed = true;

			IProcessingChainPtr processingChain = device->ProcessingChain;

			int recorderId = processingChain->AddProcessor(recordingProcessor);
			processingChain->OutputChannelPair[0]->IsActive = true;


			// start audio device
			device->Start();

			// activate processing
			transport->IsActive = true;

			TransportCode transportStatus = TransportCode::None;

			while (transportStatus != TransportCode::Stop)
			{
				// check for control input (MIDI)
				if (transport->GetNext(1000, transportStatus))
				{
					std::cout << "Received transport code: " << (int)transportStatus << std::endl;

					switch (transportStatus)
					{
					case TransportCode::Record:
						if (processingChain->InputChannel[0]->IsActive == false)
						{
							// activate in-memory recording
							processingChain->InputChannel[0]->IsActive = true;
							recordingProcessor->IsBypassed = false;
							std::cout << "Recording..." << std::endl;
						}
						else
						{
							// stop recording
							std::cout << "Switching to replay..." << std::endl;
							processingChain->InputChannel[0]->IsActive = false;

							// create audio take from current recording and start next in-memory recording
							ISampleContainerPtr take = recorder->CreateSampleContainer(true);

							//writeContents(take);
							ISampleSourcePtr takeSource = ObjectFactory::CreateContainerSource(take);
							ISampleProcessorPtr takeProcessor = ObjectFactory::CreateFromSourceProcessor(takeSource);

							// add take for replay
							int playerId = processingChain->AddProcessor(takeProcessor);
							std::cout << "Now listen..." << std::endl;
						}
						break;
					default:
						break;
					}
				}
			}

			std::cout << "Shutting down everything. Bye!" << std::endl;

			device->Stop();
		}
	}
	catch (const std::exception& error)
	{
		std::cerr << "Unhandled exception: " << error.what() << std::endl;
	}
	CoUninitialize();
}
