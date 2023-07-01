#include "pch.h"
#include "LooperEvents.h"

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

}

void LooperEvents::RecordingStatusChanged(ILooper& looper, RecordingStatusType recordingStatus)
{
}

void LooperEvents::LoopRestart(ILooper& looper)
{
}

void LooperEvents::IsLoopingChanged(ILooper& looper, bool isLooping)
{
}

void LooperEvents::IsSessionRecordingChanged(ILooper& looper, bool isSessionRecording)
{
}

void LooperEvents::AddLoop(ILooper& looper)
{
}

void LooperEvents::DropRecording(ILooper& looper, bool continueRecording)
{
}
