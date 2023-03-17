#include <iostream>
#include <conio.h>
#include <AsioCore.h>
#include <ObjectFactory.h>
#include <AsioCoreException.h>
#include "SteinbergUrRt2.h"

using namespace Audio::Asio;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;

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

		ISampleProcessorPtr recorder = ObjectFactory::CreateToContainerProcessor(_countof(selectedInputs), samplesPerTenSecs, samplesPerTenSecs);

		IInputChannelPtr input0 = device->InputChannel[0];

		ISampleSourcePtr inputSource = nullptr;
		input0->QueryInterface<ISampleSource>(&inputSource);
		inputSource->First = recorder;

		IOutputChannelPairPtr outputPair = device->OutputChannelPair[0];
		ISampleProcessorPtr outputProcessor = nullptr;
		outputPair->QueryInterface<ISampleProcessor>(&outputProcessor);

		recorder->Next = outputProcessor;

		device->Start();
		input0->IsActive = true;

		std::cout << "Recording is going on..." << std::endl;
		char key = _getch();

		std::cout << "Switching to replay..." << std::endl;
		ISampleProcessorPtr noProcessor;

		input0->IsActive = false;
		inputSource->First = noProcessor;
		recorder->Next = noProcessor;

		ISampleContainerPtr take = nullptr;
		recorder->QueryInterface<ISampleContainer>(&take);

		ISampleSourcePtr takeSource = ObjectFactory::CreateContainerSource(take, sampleCount);

		takeSource->First = outputProcessor;

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
