#include <iostream>
#include <comdef.h>
#include <AsioCore.h>
#include <SteinbergUrRt2.h>
#include <ObjectFactory.h>
#include <algorithm>
#include <array>
#include <stdexcept>

using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;


void arpeggios(IOscillatorPtr oscillator)
{
	static std::array<double, 8> intervals
	{
		1.000000,
			//1.059463,
			1.122462,
			//1.189207,
			1.259921,
			1.334840,
			//1.414214,
			1.498307,
			//1.587401,
			1.681793,
			//1.781797,
			1.887749,
			2.000000
	};

	for (int loop = 0; loop < 10; loop++)
	{
		for (const auto& interval : intervals)
		{
			oscillator->Frequency = interval * 120.0 * (double)(1 + (loop >> 1));
			oscillator->Amplitude = 0.5;
			Sleep(200);
			oscillator->Amplitude = 0.0;
			Sleep(10);
		}
	}
}

int main()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY);

	if (hr != S_OK)
	{
		std::wcerr << L"COM initialization failed." << std::endl;
		exit(-1);
	}

	bool isPrioritySet = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	if (!isPrioritySet)
	{
		std::wcerr << L"Cannot set priority class of current process to 'HIGH_PRIORITY_CLASS'." << std::endl;
	}
	else
	{
		AsioCorePtr asioCore = AsioCore::CreateInstancePtr(IID_STEINBERG_UR_RT2);

		int selectedInputs[] = { 0, 1 };
		int selectedOutputs[] = { 0, 1 };
		int sampleCount = AsioCore::UsePreferredSize;
		float outputSaturation = 1.0f;

		asioCore->CreateBuffers(selectedInputs, _countof(selectedInputs), selectedOutputs, _countof(selectedOutputs), sampleCount, outputSaturation);


		IProcessingChainPtr processingChain = asioCore->ProcessingChain;

		//IOscillatorPtr oscillator = ObjectFactory::CreateTestOscillator(asioCore->SampleRate, 0.0, 0.0);
		//ISampleProcessorPtr oscProcessor = nullptr;
		//oscillator->QueryInterface<ISampleProcessor>(&oscProcessor);
		//processingChain->AddProcessor(oscProcessor);

		//ISampleProcessorPtr recordingProcessor = ObjectFactory::CreateToFileProcessor(L"some_test.raw");
		//processingChain->AddProcessor(recordingProcessor);

		ISampleSourcePtr source = ObjectFactory::CreateRawFileSource(L"some_test.raw", 1);
		ISampleProcessorPtr fileProcessor = ObjectFactory::CreateFromSourceProcessor(source);
		processingChain->AddProcessor(fileProcessor);


		processingChain->OutputChannelPair[0]->IsActive = true;
		processingChain->InputChannel[0]->IsActive = false;


		asioCore->Start();

		//arpeggios(oscillator);
		Sleep(30000);

		asioCore->Stop();
	}

	CoUninitialize();
}

