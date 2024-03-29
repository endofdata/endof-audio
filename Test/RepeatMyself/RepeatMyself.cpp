#include "pch.h"
#include <initguid.h>
#include <AsioDebugDriverGuid.h>
#include "SteinbergUrRt2.h"
#include <AsioCore.h>
#include <FoundationObjectFactory.h>
#include <AsioObjectFactory.h>
#include <StrConv.h>

#include "LooperEvents.h"

using namespace Audio::Asio;
using namespace Audio::Vst::Unmanaged;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;

bool onDeviceCaps(unsigned int id, const MIDIINCAPS& devcaps, void* callbackParam)
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

static void addVstFx(ILooper* looper)
{
	const wchar_t* pwcszLibName = L"C:\\Program Files\\Common Files\\VST3\\Unfiltered Audio Indent.vst3";
	// const wchar_t* pwcszLibName = L"D:\\Gamer I5\\Documents\\Projects\\vst3sdk\\out\\build\\x64-Debug\\VST3\\Debug\\adelay.vst3\\Contents\\x86_64-win\\adelay.vst3";
	// const wchar_t* pwcszLibName = L"D:\\Gamer I5\\Documents\\Projects\\vst3sdk\\out\\build\\x64-Debug\\VST3\\Debug\\panner.vst3\\Contents\\x86_64-win\\panner.vst3";
	// const wchar_t* pwcszLibName = L"D:\\Gamer I5\\Documents\\Projects\\vst3sdk\\out\\build\\x64-Debug\\VST3\\Debug\\hostchecker.vst3\\Contents\\x86_64-win\\hostchecker.vst3";
	const wchar_t* pluginIdRaw = looper->AddVstPlugin(pwcszLibName);

	looper->InsertFx(pluginIdRaw);
}

static void runLooper(const ILooperConfigPtr& config)
{
	ILooperPtr looper = AsioObjectFactory::CreateLooper(config);
	ILooperEventsPtr looperEvents = new RepeatMyself::LooperEvents();

	try
	{
		looper->LooperEvents = looperEvents;
		addVstFx(looper);

		for (int i = 0; i < static_cast<int>(config->InputChannelCount); i++)
		{
			looper->SelectInput(config->InputChannel[i], true);
		}
		for (int i = 0; i < static_cast<int>(config->OutputChannelCount); i += 2)
		{
			int pair[2] = { config->OutputChannel[i], config->OutputChannel[i + 1] };
			looper->SelectOutputPair(pair, true);
		}

		// create master recording
		looper->IsSessionRecording = true;

		std::wcout << L"Running the looper" << std::endl;

		looper->Run();

		if (looper->IsSessionRecording)
		{
			std::wostringstream builder;
			SYSTEMTIME st;
			GetSystemTime(&st);
			builder << L"sessions\\"
				<< std::setw(4) << st.wYear
				<< std::setw(2) << std::setfill(L'0') << st.wMonth 
				<< std::setw(2) << std::setfill(L'0') << st.wDay << L"_"
				<< std::setw(2) << std::setfill(L'0') << st.wHour 
				<< std::setw(2) << std::setfill(L'0') << st.wMinute 
				<< std::setw(2) << std::setfill(L'0') << st.wSecond << L"_";
			std::wstring filenameBase = builder.str();

			std::wcout << L"Writing session to file set '" << filenameBase << L"*'." << std::endl;

			looper->SaveSession(filenameBase.c_str());
		}
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
	int selectedInputs[] = { 0, 1 };
	int selectedOutputs[] = { 0, 1 };
	int sampleCount = 512;
	float outputSaturation = 0.5f;

	bool isPrioritySet = SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	if (isPrioritySet == FALSE)
	{
		std::wcerr << L"Cannot set process priority class to 'high'.";
	}

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY);

	if (hr != S_OK)
	{
		std::wcerr << L"COM initialization failed." << std::endl;
		exit(-1);
	}

	try
	{
		int midiInId = FoundationObjectFactory::SelectMidiInputDevice(onDeviceCaps);

		if (midiInId < 0)
		{
			std::wcerr << L"Failed to select MIDI input device." << std::endl;
		}
		else
		{
			ILooperConfigPtr looperConfig = AsioObjectFactory::CreateLooperConfiguration();

			looperConfig->Name = L"RepeatMyself";
			looperConfig->MidiInput = midiInId;
			looperConfig->AsioDevice = IID_STEINBERG_UR_RT2;
			//looperConfig.AsioDevice = CLSID_AsioDebugDriver;
			looperConfig->AddInputChannelList(selectedInputs, _countof(selectedInputs));
			looperConfig->AddOutputChannelList(selectedOutputs, _countof(selectedOutputs));

			// optional
			looperConfig->SampleCount = AsioCore::UsePreferredSize;
			looperConfig->OutputSaturation = 1.0f;

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