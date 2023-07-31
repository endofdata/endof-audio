#include "pch.h"
#include "LooperEvents.h"
#include "Looper.h"

using namespace Audio::Asio::Interop;

LooperEvents::LooperEvents(Looper^ looper) :
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
	m_looper->OnPropertyChanged(Looper::LoopPositionProperty);
}

void LooperEvents::RecordingStatusChanged(ILooper& looper, RecordingStatusType recordingStatus)
{
	m_looper->OnPropertyChanged(Looper::RecordingStatusProperty);
}

void LooperEvents::LoopRestart(ILooper& looper)
{
	m_looper->OnPropertyChanged(Looper::LoopPositionProperty);
}

void LooperEvents::IsLoopingChanged(ILooper& looper, bool isLooping)
{
	m_looper->OnPropertyChanged(Looper::IsLoopingProperty);
}

void LooperEvents::IsSessionRecordingChanged(ILooper& looper, bool isSessionRecording)
{
	m_looper->OnPropertyChanged(Looper::IsSessionRecordingProperty);
}

void LooperEvents::AddLoop(ILooper& looper)
{
	m_looper->OnPropertyChanged(Looper::LoopCountProperty);
}

void LooperEvents::DropRecording(ILooper& looper)
{
	m_looper->OnPropertyChanged(Looper::LoopCountProperty);
}
