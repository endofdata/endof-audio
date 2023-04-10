#include "pch.h"
#include "Transport.h"

using namespace Audio::Foundation::Unmanaged;


Transport::Transport(IHostClockPtr& hostClock, ITransportEventsPtr& events) :
	m_hostClock(hostClock),
	m_events(events),
	m_loopStartPosition(0),
	m_loopEndPosition(0),
	m_isLooping(false),
	m_isSkipping(false),
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
	m_currentSamplePosition = AudioTimeToSamplePosition(m_hostClock->CurrentTime);
	m_events->Transport(TransportCode::Start);
}

void Transport::Stop()
{
	m_hostClock->Stop();
	m_events->Transport(TransportCode::Stop);
}

void Transport::Pulse(int sampleCount)
{
	m_currentSamplePosition += sampleCount;
	m_isSkipping = false;

	if (m_isLooping)
	{
		if (m_currentSamplePosition >= m_loopEndPosition)
		{
			m_currentSamplePosition = m_loopStartPosition;
			m_hostClock->CurrentTime = SamplePositionToAudioTime(m_currentSamplePosition);
			m_isSkipping = true;
			m_events->Transport(TransportCode::Locate);
		}
	}
}

int Transport::get_CurrentSamplePosition() const
{
	return m_currentSamplePosition;
}

void Transport::put_CurrentSamplePosition(int value)
{
	m_currentSamplePosition = value;
}


bool Transport::get_IsSkipping()
{
	return m_isSkipping;
}

bool Transport::get_IsLooping()
{
	return m_isLooping;
}

void Transport::put_IsLooping(bool value)
{
	m_isLooping = value;
}

AudioTime Transport::get_LoopStart()
{
	return SamplePositionToAudioTime(m_loopStartPosition);
}

void Transport::put_LoopStart(AudioTime value)
{
	int samplePosition = AudioTimeToSamplePosition(value);

	if (samplePosition > m_loopEndPosition)
	{
		m_loopStartPosition = m_loopEndPosition;
		m_loopEndPosition = samplePosition;
	}
	else
	{
		m_loopStartPosition = samplePosition;
	}
}

AudioTime Transport::get_LoopEnd()
{
	return SamplePositionToAudioTime(m_loopEndPosition);
}

void Transport::put_LoopEnd(AudioTime value)
{
	int samplePosition = AudioTimeToSamplePosition(value);

	if (samplePosition < m_loopStartPosition)
	{
		m_loopEndPosition = m_loopStartPosition;
		m_loopStartPosition = samplePosition;
	}
	else
	{
		m_loopEndPosition = samplePosition;
	}
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
