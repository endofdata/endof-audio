#include "pch.h"
#include <initguid.h>
#include <AsioDebugDriverGuid.h>
#include "SteinbergUrRt2.h"
#include "CommandLine.h"
#include <AsioCore.h>
#include <FoundationObjectFactory.h>
#include <AsioObjectFactory.h>
#include <StrConv.h>

#include "LooperEvents.h"

using namespace RepeatMyself;
using namespace Audio::Asio;
using namespace Audio::Vst::Unmanaged;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged;

bool onDeviceCaps(unsigned int id, const MIDIINCAPS& devcaps, void* callbackParam)
{
	const CommandLine& commandLine = *reinterpret_cast<CommandLine*>(callbackParam);
	std::string name = StrConv::ToUtf8(devcaps.szPname);

	std::cout << "Input ID " << id << " '" << name.c_str() << "'" << std::endl;

	bool isSelected = name == commandLine.MidiDevice;

	if (isSelected)
	{
		std::cout << "Selected MIDI input device: '" << commandLine.MidiDevice << "', ID '" << id << "'" << std::endl;
	}

	return isSelected;
}

IController* createController(ITransportPtr& transport, void* param)
{
	int midiInput = reinterpret_cast<int>(param);

	IController* controller = FoundationObjectFactory::CreateMidiTransportControl(transport, midiInput).Detach();

	if (controller == nullptr)
	{
		throw std::runtime_error("Initialization of Controller failed.");
	}
	return controller;
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

static ILooper* _current_looper = NULL;

static void runLooper(const ILooperConfigPtr& config, const CommandLine& commandLine)
{
	ILooperPtr looper = AsioObjectFactory::CreateLooper(config);

	ILooperEventsPtr looperEvents = new RepeatMyself::LooperEvents();

	looper->LooperEvents = looperEvents;
	//addVstFx(looper);

	for (int i = 0; i < static_cast<int>(config->InputChannelCount); i++)
	{
		looper->SelectInput(i, true);
	}
	for (int o = 0; o < static_cast<int>(config->OutputChannelCount) / 2; o++)
	{
		looper->SelectOutputPair(o, true);
	}

	// create master recording, if requested
	looper->IsSessionRecording = commandLine.IsSessionRecording;

	ControllerMapping mappings[] =
	{
		ControllerMapping(ControllerCode::Locate, 70),
		ControllerMapping(ControllerCode::Pause, 71),
		ControllerMapping(ControllerCode::Cancel, 73),
		ControllerMapping(ControllerCode::Record, 74),
		ControllerMapping(ControllerCode::Run, 80),
		ControllerMapping(ControllerCode::Stop, 81)
	};

	looper->Controller->ConfigureMappings(mappings, _countof(mappings));

	std::wcout << L"Running the looper" << std::endl;

	looper.AddRef();
	_current_looper = looper.GetInterfacePtr();
	looper->Start();

	looper->Wait(INFINITE);
	_current_looper = nullptr;
	looper->Release();

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

BOOL WINAPI consoleHandler(DWORD signal)
{
	if (signal == CTRL_C_EVENT)
	{
		ILooper* looper = _current_looper;

		if (looper != NULL)
		{
			looper->Stop(INFINITE);
			std::wcout << L"Looper stopped with CTRL-C" << std::endl;
		}
	}
	return TRUE;
}

int main(int argc, char* argv[])
{
	CommandLine commandLine = CommandLine::FromArgs(argc, argv);

	if (!SetConsoleCtrlHandler(consoleHandler, TRUE))
	{
		std::wcerr << L"Cannot not set console control handler";
		return -1;
	}

	bool isPrioritySet = SetPriorityClass(GetCurrentProcess(), commandLine.Priority);

	if (isPrioritySet == FALSE)
	{
		std::wcerr << L"Cannot set process priority class.";
		return -1;
	}

	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_SPEED_OVER_MEMORY);

	if (hr != S_OK)
	{
		std::wcerr << L"COM initialization failed." << std::endl;
		exit(-1);
	}

	try
	{
		int midiInId = FoundationObjectFactory::SelectMidiInputDevice(onDeviceCaps, reinterpret_cast<void*>(&commandLine));

		if (midiInId < 0)
		{
			std::wcerr << L"Failed to select MIDI input device." << std::endl;
		}
		else
		{
			ILooperConfigPtr looperConfig = AsioObjectFactory::CreateLooperConfiguration();
			

			looperConfig->Name = L"RepeatMyself";
			//looperConfig->MidiInput = midiInId;
			looperConfig->ControllerFactory = createController;
			looperConfig->ControllerFactoryParam = reinterpret_cast<void*>(midiInId);
			looperConfig->AsioDevice = IID_STEINBERG_UR_RT2;
			//looperConfig.AsioDevice = CLSID_AsioDebugDriver;
			looperConfig->AddInputChannelList(commandLine.InputList, commandLine.InputCount);
			looperConfig->AddOutputChannelList(commandLine.OutputList, commandLine.OutputCount);

			// optional
			looperConfig->SampleCount = AsioCore::UsePreferredSize;
			looperConfig->OutputSaturation = commandLine.OutputSaturation;

			runLooper(looperConfig, commandLine);

			std::wcout << L"Shutting down everything. Bye!" << std::endl;
		}
	}
	catch (const std::exception& error)
	{
		std::wcerr << L"Unhandled exception: " << error.what() << std::endl;
	}
	CoUninitialize();
}