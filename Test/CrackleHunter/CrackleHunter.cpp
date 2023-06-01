#include <iostream>
#include <comdef.h>
#include <AsioCore.h>
#include <SteinbergUrRt2.h>

using namespace Audio::Asio::Unmanaged;

int main()
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY);

	if (hr != S_OK)
	{
		std::wcerr << L"COM initialization failed." << std::endl;
		exit(-1);
	}

	bool isPrioritySet = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

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

