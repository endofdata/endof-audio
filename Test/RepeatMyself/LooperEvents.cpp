#include "pch.h"
#include "LooperEvents.h"

using namespace RepeatMyself;

const wchar_t* ClearLine = L"\33[2K\r";

LooperEvents::LooperEvents() :
	m_loopCount(0),
	m_refCount(0)
{
}

LooperEvents::~LooperEvents()
{
}

IMPLEMENT_IUNKNOWN(LooperEvents)

void* LooperEvents::GetInterface(REFIID iid)
{
	if (iid == _uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == _uuidof(ILooperEvents))
	{
		return dynamic_cast<ILooperEvents*>(this);
	}
	return nullptr;
}

void LooperEvents::Heartbeat(ILooper& looper, ITransportPtr& transport)
{
	std::wcout << transport->TimePosition.ToString() << L"\r";
}

void LooperEvents::RecordingStatusChanged(ILooper& looper, RecordingStatusType recordingStatus)
{
	std::wcout << L"Recording status: " << GetRecordingStatusString(recordingStatus)  << std::endl;
}

void LooperEvents::LoopRestart(ILooper& looper)
{
	std::wcout << ClearLine << L"Loop " << m_loopCount++ << L"\r";
}

void LooperEvents::IsLoopingChanged(ILooper& looper, bool isLooping)
{
	std::wcout << ClearLine << L"Loop recording: " << std::boolalpha << isLooping << std::endl;
}

void LooperEvents::IsSessionRecordingChanged(ILooper& looper, bool isSessionRecording)
{
	//std::wcout << ClearLine << L"Session recording: " << std::boolalpha << isSessionRecording << std::endl;
}

void LooperEvents::AddLoop(ILooper& looper, int channelCount, int samplePosition, int sampleCount)
{
	std::wcout << ClearLine << L"Added loop (channels: " << channelCount << L", pos: " << samplePosition << L", len: " << sampleCount << L")" << std::endl;
}

void LooperEvents::DropRecording(ILooper& looper)
{
	std::wcout << ClearLine << L"Dropped recording" << std::endl;
}

const wchar_t* LooperEvents::GetTransportCodeString(ControllerCode value)
{
	switch (value)
	{
	case ControllerCode::Run:
		return L"Run";
	case ControllerCode::Stop:
		return L"Stop";
	case ControllerCode::RecordArm:
		return L"RecordArm";
	case ControllerCode::Record:
		return L"Record";
	case ControllerCode::Locate:
		return L"Locate";
	case ControllerCode::None:
		return L"None";
	default:
		return L"<unknown>";
	}
}

const wchar_t* LooperEvents::GetRecordingStatusString(RecordingStatusType value)
{
	switch (value)
	{
	case RecordingStatusType::Off:
		return L"Off";
	case RecordingStatusType::Armed:
		return L"Armed";
	case RecordingStatusType::Recording:
		return L"Recording";
	case RecordingStatusType::Unarmed:
		return L"Unarmed";
	default:
		return L"<unknown>";
	}
}