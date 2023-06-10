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
	ProcessingContext& context = transport->Context;

	int loopSamples = context.LoopEndSample - context.LoopStartSample;

	m_looper->LoopLength = loopSamples;
	m_looper->LoopPosition = loopSamples != 0 ? ((float)context.SamplePosition / (float)loopSamples) : 0.0f;
}

void LooperEvents::TransportStatusChanged(ILooper& looper, TransportCode previous, TransportCode current)
{
	m_looper->OnTransportStatusChanged(previous, current);
}

void LooperEvents::LoopRestart(ILooper& looper)
{
	m_looper->OnLoopRestart();
}

void LooperEvents::LoopRecordingChanged(ILooper& looper, bool isLoopRecording)
{
	m_looper->IsLoopRecording = isLoopRecording;
}

void LooperEvents::SessionRecordingChanged(ILooper& looper, bool isSessionRecording)
{
	m_looper->IsSessionRecording = isSessionRecording;
}

void LooperEvents::AddLoop(ILooper& looper)
{
	m_looper->LoopCount += 1;
}

void LooperEvents::DropRecording(ILooper& looper, bool continueRecording)
{
	m_looper->LoopCount -= 1;
}
