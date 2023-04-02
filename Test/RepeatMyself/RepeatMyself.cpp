#include <iostream>
#include <conio.h>
#include <fstream>
#include <strstream>
#include <ios>
#include <AsioCore.h>
#include <ObjectFactory.h>
#include <AsioCoreException.h>
#include "SteinbergUrRt2.h"

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
		std::cout << "COM initialization failed." << std::endl;
		exit(-1);
	}

	try
	{
		AsioCorePtr device = nullptr;

		try
		{
			device = AsioCore::CreateInstancePtr(IID_STEINBERG_UR_RT2);
		}
		catch (const AsioCoreException& acx)
		{
			std::cout << "Failed to create device. " << acx.Message << " Error code: " << std::hex << acx.Error << std::endl;
			exit(-1);
		}

		device->CreateBuffers(selectedInputs, _countof(selectedInputs), selectedOutputs, _countof(selectedOutputs), sampleCount);

		float samplesPerTenSecs = device->SampleRate * 60.0f;

		// create processor chain input -> recorder -> output
		ISampleProcessorPtr recorder = ObjectFactory::CreateToContainerProcessor(_countof(selectedInputs), samplesPerTenSecs, samplesPerTenSecs);

		IProcessingChainPtr processingChain = device->ProcessingChain;

		int recorderId = processingChain->AddProcessor(recorder);
		processingChain->InputChannel[0]->IsActive = true;
		processingChain->OutputChannelPair[0]->IsActive = true;

		// begin recording until a key is pressed
		device->Start();

		std::cout << "Recording is going on..." << std::endl;
		char key = _getch();

		// deactivate input
		std::cout << "Switching to replay..." << std::endl;

		processingChain->InputChannel[0]->IsActive = false;

		ISampleContainerPtr take = nullptr;
		recorder->QueryInterface<ISampleContainer>(&take);

		writeContents(take);

		ISampleSourcePtr takeSource = ObjectFactory::CreateContainerSource(take);
		ISampleProcessorPtr takeProcessor = ObjectFactory::CreateFromSourceProcessor(takeSource);

		processingChain->RemoveProcessor(recorderId);
		int playerId = processingChain->AddProcessor(takeProcessor);

		std::cout << "Now listen..." << std::endl;

		key = _getch();

		std::cout << "Shutting down everything. Bye!" << std::endl;

		device->Stop();
	}
	catch (const std::exception& error)
	{
		std::cout << "Unhandled exception: " << error.what() << std::endl;
	}
	CoUninitialize();
}
