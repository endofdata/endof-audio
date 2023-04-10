#include "pch.h"
#include "HostClock.h"

using namespace Audio::Foundation::Unmanaged;


HostClock::HostClock(double sampleRate) :
	m_offset(std::chrono::steady_clock::now()),
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

AudioTime HostClock::get_CurrentTime() const
{
	return AudioTime(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_offset));
}

void HostClock::put_CurrentTime(const AudioTime& position)
{
	AudioTime currentTime = CurrentTime;
	auto delta = std::chrono::microseconds(currentTime - position);
	m_offset += std::chrono::steady_clock::duration(delta);
}

void HostClock::Start()
{
	m_offset = std::chrono::steady_clock::now();
}

void HostClock::Stop()
{
}

double HostClock::get_SampleRate() const
{
	return m_sampleRate;
}

void HostClock::put_SampleRate(double value)
{
	m_sampleRate = value;
}
