#include "pch.h"
#include "Transport.h"

using namespace Audio::Foundation::Unmanaged;


Transport::Transport(IHostClockPtr& hostClock) :
	m_hostClock(hostClock),
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
}

void Transport::Stop()
{
	m_hostClock->Stop();
}

void Transport::Pulse()
{
	m_isSkipping = false;

	if (m_isLooping)
	{
		if (m_hostClock->CurrentTime >= m_loopEnd)
		{
			m_hostClock->CurrentTime = m_loopStart;
			m_isSkipping = true;
		}
	}
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
	return m_loopStart;
}

void Transport::put_LoopStart(AudioTime value)
{
	if (value > m_loopEnd)
	{
		m_loopStart = m_loopEnd;
		m_loopEnd = value;
	}
	else
	{
		m_loopStart = value;
	}
}

AudioTime Transport::get_LoopEnd()
{
	return m_loopEnd;
}

void Transport::put_LoopEnd(AudioTime value)
{
	if (value < m_loopStart)
	{
		m_loopEnd = m_loopStart;
		m_loopStart = value;
	}
	else
	{
		m_loopEnd = value;
	}
}

IHostClockPtr& Transport::get_HostClock()
{
	return m_hostClock;
}
