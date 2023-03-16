#include "pch.h"
#include "HostClock.h"

using namespace Audio::Foundation::Unmanaged;


HostClock::HostClock(int sampleRate) :
	m_sampleRate(sampleRate),
	m_refCount(0)
{
}

HostClock::~HostClock()
{
}

IMPLEMENT_IUNKNOWN(HostClock)

bool HostClock::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}

	if (iid == __uuidof(IHostClock))
	{
		*ppvResult = dynamic_cast<IHostClock*>(this);
		return true;
	}
	return false;
}

void HostClock::Start()
{
	m_offset = std::chrono::steady_clock::now();
}

void HostClock::Stop()
{
}

AudioTime HostClock::get_CurrentTime() const
{
	return static_cast<AudioTime>((std::chrono::steady_clock::now() - m_offset).count());
}

int HostClock::get_SampleRate() const
{
	return m_sampleRate;
}