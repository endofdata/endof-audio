#include "pch.h"
#include <initguid.h>
#include <AsioDebugDriverGuid.h>
#include "SteinbergUrRt2.h"

#include <ObjectFactory.h>
#include <StrConv.h>

#include "Looper.h"
#include "LooperConfig.h"

using namespace Audio::Asio;
using namespace Audio::Vst::Unmanaged;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;

const wchar_t* ClearLine = L"\33[2K\r";

bool onDeviceCaps(unsigned int id, const MIDIINCAPS& devcaps)
{
	std::string name = StrConv::ToUtf8(devcaps.szPname);

	std::cout << "Input ID " << id << " '" << name.c_str() << "'" << std::endl;

	bool isSelected = name == "2- Steinberg UR-RT2-1";
	//bool isSelected = name == "LoopBe Internal MIDI";

	if (isSelected)
	{
		std::cout << "Selected MIDI input device ID '" << id << "'" << std::endl;
	}

	return isSelected;
}

static void addVstFx(Looper* looper)
{
	const wchar_t* pwcszLibName = L"C:\\Program Files\\Common Files\\VST3\\Unfiltered Audio Indent.vst3";
	// const wchar_t* pwcszLibName = L"D:\\Gamer I5\\Documents\\Projects\\vst3sdk\\out\\build\\x64-Debug\\VST3\\Debug\\adelay.vst3\\Contents\\x86_64-win\\adelay.vst3";
	// const wchar_t* pwcszLibName = L"D:\\Gamer I5\\Documents\\Projects\\vst3sdk\\out\\build\\x64-Debug\\VST3\\Debug\\panner.vst3\\Contents\\x86_64-win\\panner.vst3";
	// const wchar_t* pwcszLibName = L"D:\\Gamer I5\\Documents\\Projects\\vst3sdk\\out\\build\\x64-Debug\\VST3\\Debug\\hostchecker.vst3\\Contents\\x86_64-win\\hostchecker.vst3";
	const wchar_t* pluginIdRaw = looper->AddVstPlugin(pwcszLibName);

	looper->InsertFx(pluginIdRaw);
}

static void runLooper(const LooperConfig& config)
{
	Looper* looper = Looper::Create(config, L"RepeatMyself");

	try
	{
		// addVstFx(looper);

		for (size_t i = 0; i < config.InputChannelCount; i++)
		{
			looper->SelectInput(config.InputChannel[i], true);
		}
		for (size_t i = 0; i < config.OutputChannelCount; i += 2)
		{
			int pair[2] = { config.OutputChannel[i], config.OutputChannel[i + 1] };
			looper->SelectOutputPair(pair, true);
		}
		looper->Run();
	}
	catch (const std::exception&)
	{
		looper->Release();
		throw;
	}
	looper->Release();
}

int main()
{
	int selectedInputs[] = { 0 };
	int selectedOutputs[] = { 0, 1 };
	int sampleCount = 512;
	float outputSaturation = 0.5f;

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	if (hr != S_OK)
	{
		std::wcerr << L"COM initialization failed." << std::endl;
		exit(-1);
	}

	try
	{
		int midiInId = ObjectFactory::SelectMidiInputDevice(onDeviceCaps);

		if (midiInId < 0)
		{
			std::wcerr << L"Failed to select MIDI input device." << std::endl;
		}
		else
		{
			LooperConfig looperConfig;

			looperConfig.MidiInput = midiInId;
			looperConfig.AsioDevice = IID_STEINBERG_UR_RT2;
			//looperConfig.AsioDevice = CLSID_AsioDebugDriver;
			looperConfig.AddInputChannelList(selectedInputs, 1);
			looperConfig.AddOutputChannelList(selectedOutputs, 2);

			// optional
			//looperConfig.SampleCount = 512;
			//looperConfig.OutputSaturation = 0.5f;

			runLooper(looperConfig);

			std::wcout << L"Shutting down everything. Bye!" << std::endl;
		}
	}
	catch (const std::exception& error)
	{
		std::wcerr << L"Unhandled exception: " << error.what() << std::endl;
	}
	CoUninitialize();
}