#include "pch.h"
#include "Transport.h"

using namespace Audio::Foundation::Unmanaged;


Transport::Transport(IHostClockPtr& hostClock, IControllerEventsPtr& events, int sampleCount) :
	m_hostClock(hostClock),
	m_context(sampleCount),
	m_events(events),
	m_isPaused(0),
	m_refCount(0)
{
}

Transport::~Transport()
{
}

IMPLEMENT_IUNKNOWN(Transport)

void* Transport::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}

	if (iid == __uuidof(ITransport))
	{
		return dynamic_cast<ITransport*>(this);
	}
	return nullptr;
}

void Transport::Run()
{
	m_isPaused = false;
	m_hostClock->Start();
	m_context.SamplePosition = AudioTimeToSamplePosition(m_hostClock->CurrentTime);
	m_events->ControllerCommand(ControllerCode::Run);
}

bool Transport::get_IsPaused() const
{
	return static_cast<bool>(m_isPaused);
}

void Transport::put_IsPaused(bool value)
{
	unsigned int oldValue = static_cast<bool>(::InterlockedExchange(&m_isPaused, value ? 1 : 0));

	if (oldValue != m_isPaused)
	{
		if (m_isPaused)
		{
			m_hostClock->Stop();
		}
		else
		{
			m_hostClock->Start();
		}
	}
}

void Transport::Stop()
{
	m_isPaused = false;
	m_hostClock->Stop();
	m_events->ControllerCommand(ControllerCode::Stop);
}

ProcessingContext& Transport::Pulse()
{
	if (!m_isPaused)
	{
		m_context.SamplePosition += m_context.SampleCount;

		if (m_context.IsLooping)
		{
			if (m_context.IsLoopStart)
			{
				m_hostClock->CurrentTime = LoopStartTime;
				m_events->ControllerCommand(ControllerCode::Locate);
			}
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

IControllerEventsPtr& Transport::get_Events()
{
	return m_events;
}

int Transport::AudioTimeToSamplePosition(const AudioTime& value) const
{
	return static_cast<int>(static_cast<double>(value.Value) * m_hostClock->SampleRate / 10000000.0);
}

AudioTime Transport::SamplePositionToAudioTime(int value) const
{
	return AudioTime(static_cast<int>(static_cast<double>(value) / m_hostClock->SampleRate * 10000000.0));
}
