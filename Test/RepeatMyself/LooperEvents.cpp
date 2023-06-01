#include "pch.h"
#include "LooperEvents.h"

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

void LooperEvents::TransportStatusChanged(ILooper& looper, TransportCode previous, TransportCode current)
{
	std::wcout << L"Transport status: " << GetTransportCodeString(previous) << L" > " << GetTransportCodeString(current) << std::endl;
}

void LooperEvents::LoopRestart(ILooper& looper)
{
	std::wcout << ClearLine << L"Loop " << m_loopCount++ << L"\r";
}

void LooperEvents::LoopRecordingChanged(ILooper& looper, bool isLoopRecording)
{
	std::wcout << ClearLine << L"Recording: " << std::boolalpha << isLoopRecording << std::endl;
}

void LooperEvents::SessionRecordingChanged(ILooper& looper, bool isSessionRecording)
{
	//std::wcout << ClearLine << L"Session recording: " << std::boolalpha << isSessionRecording << std::endl;
}

void LooperEvents::AddLoop(ILooper& looper)
{
	std::wcout << ClearLine << L"Added loop" << std::endl;
}

void LooperEvents::DropRecording(ILooper& looper, bool continueRecording)
{
	std::wcout << ClearLine << L"Dropped recording" << std::endl;
}

const wchar_t* LooperEvents::GetTransportCodeString(TransportCode value)
{
	const wchar_t* pwcszStatus = nullptr;

	switch (value)
	{
	case TransportCode::Start:
		pwcszStatus = L"Start";
		break;
	case TransportCode::Stop:
		pwcszStatus = L"Stop";
		break;
	case TransportCode::Record:
		pwcszStatus = L"Record";
		break;
	case TransportCode::Locate:
		pwcszStatus = L"Locate";
		break;
	case TransportCode::None:
		pwcszStatus = L"None";
		break;
	default:
		pwcszStatus = L"<unknown>";
		break;
	}
	return pwcszStatus;
}