#include "pch.h"
#include "Transport.h"

using namespace Audio::Foundation::Unmanaged;


Transport::Transport(IHostClockPtr& hostClock, ITransportEventsPtr& events, int sampleCount) :
	m_hostClock(hostClock),
	m_context(sampleCount),
	m_events(events),
	m_refCount(0)
{
}

Transport::~Transport()
{
}

IMPLEMENT_IUNKNOWN(Transport)

bool Transport::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}

	if (iid == __uuidof(ITransport))
	{
		*ppvResult = dynamic_cast<ITransport*>(this);
		return true;
	}
	return false;
}

void Transport::Start()
{
	m_hostClock->Start();
	m_context.SamplePosition = AudioTimeToSamplePosition(m_hostClock->CurrentTime);
	m_events->Transport(TransportCode::Start);
}

void Transport::Stop()
{
	m_hostClock->Stop();
	m_events->Transport(TransportCode::Stop);
}

ProcessingContext& Transport::Pulse()
{
	m_context.SamplePosition += m_context.SampleCount;

	if (m_context.IsLooping)
	{
		if (m_context.IsLoopStart)
		{
			m_hostClock->CurrentTime = LoopStartTime;
			m_events->Transport(TransportCode::Locate);
		}
	}
	return m_context;
}

ProcessingContext& Transport::get_Context()
{
	return m_context;
}

AudioTime Transport::get_TimePosition() const
{
	return SamplePositionToAudioTime(m_context.SamplePosition);
}

void Transport::put_TimePosition(AudioTime value)
{
	m_context.SamplePosition = AudioTimeToSamplePosition(value);
}

AudioTime Transport::get_LoopStartTime()
{
	return SamplePositionToAudioTime(m_context.LoopStartSample);
}

void Transport::put_LoopStartTime(AudioTime value)
{
	m_context.LoopStartSample = AudioTimeToSamplePosition(value);
}

AudioTime Transport::get_LoopEndTime()
{
	return SamplePositionToAudioTime(m_context.LoopEndSample);
}

void Transport::put_LoopEndTime(AudioTime value)
{
	m_context.LoopEndSample = AudioTimeToSamplePosition(value);
}

IHostClockPtr& Transport::get_HostClock()
{
	return m_hostClock;
}

ITransportEventsPtr& Transport::get_Events()
{
	return m_events;
}

int Transport::AudioTimeToSamplePosition(const AudioTime& value) const
{
	return static_cast<int>(static_cast<double>(value.Value) * m_hostClock->SampleRate / 1000000.0);
}

AudioTime Transport::SamplePositionToAudioTime(int value) const
{
	return AudioTime(static_cast<int>(static_cast<double>(value) / m_hostClock->SampleRate * 1000000.0));
}
