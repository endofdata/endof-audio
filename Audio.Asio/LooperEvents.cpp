#include "pch.h"
#include "LooperEvents.h"
#include "ManagedLooper.h"

using namespace Audio::Asio;
using namespace Audio::Asio::Interop;

LooperEvents::LooperEvents(ManagedLooper^ looper) :
	m_looper(looper),
	m_refCount(0)
{
}

LooperEvents::~LooperEvents()
{
	m_looper = nullptr;
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
	m_looper->TransportPosition->Value = transport->TimePosition.Value;
	m_looper->OnPropertyChanged(ManagedLooper::TransportPositionProperty);
}

void LooperEvents::RecordingStatusChanged(ILooper& looper, RecordingStatusType recordingStatus)
{
	m_looper->OnPropertyChanged(ManagedLooper::RecordingStatusProperty);
}

void LooperEvents::LoopRestart(ILooper& looper)
{
	m_looper->OnPropertyChanged(ManagedLooper::LoopPositionProperty);
}

void LooperEvents::IsLoopingChanged(ILooper& looper, bool isLooping)
{
	m_looper->OnPropertyChanged(ManagedLooper::IsLoopingProperty);
}

void LooperEvents::IsSessionRecordingChanged(ILooper& looper, bool isSessionRecording)
{
	m_looper->OnPropertyChanged(ManagedLooper::IsSessionRecordingProperty);
}

void LooperEvents::AddLoop(ILooper& looper)
{
	m_looper->OnPropertyChanged(ManagedLooper::LoopCountProperty);
}

void LooperEvents::DropRecording(ILooper& looper)
{
	m_looper->OnPropertyChanged(ManagedLooper::LoopCountProperty);
}
