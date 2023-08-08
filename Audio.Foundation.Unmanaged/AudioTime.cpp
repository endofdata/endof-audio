#include "pch.h"
#include "AudioTime.h"
#include <algorithm>
#include <string>
#include <sstream>

using namespace Audio::Foundation::Unmanaged;

AudioTime AudioTime::FromSeconds(double seconds)
{
	return AudioTime(static_cast<long long>(seconds * 1000000.0));
}

AudioTime::AudioTime() :
	m_lower(),
	m_upper(),
	m_ticks(0)
{
}

AudioTime::AudioTime(const std::chrono::microseconds& micros) :
	m_lower(),
	m_upper(),
	m_ticks(micros.count())
{
}

AudioTime::AudioTime(const long long value) :
	m_lower(),
	m_upper(),
	m_ticks(value)
{
}

AudioTime::operator long long() const
{
	return m_ticks;
}

AudioTime& AudioTime::operator + (const AudioTime& other)
{
	Value += other.m_ticks;
	return *this;
}

AudioTime& AudioTime::operator - (const AudioTime& other)
{
	Value += other.m_ticks;
	return *this;
}

AudioTime& AudioTime::operator += (const AudioTime& other)
{
	Value += other.m_ticks;
	return *this;
}

AudioTime& AudioTime::operator -= (const AudioTime& other)
{
	Value -= other.m_ticks;
	return *this;
}

bool AudioTime::operator == (const AudioTime& other) const
{
	return m_ticks == other.m_ticks;
}

bool AudioTime::operator != (const AudioTime& other) const
{
	return m_ticks != other.m_ticks;
}

bool AudioTime::operator > (const AudioTime& other) const
{
	return m_ticks > other.m_ticks;
}

bool AudioTime::operator < (const AudioTime& other) const
{
	return m_ticks < other.m_ticks;
}

bool AudioTime::operator >= (const AudioTime& other) const
{
	return m_ticks >= other.m_ticks;
}

bool AudioTime::operator <= (const AudioTime& other) const
{
	return m_ticks <= other.m_ticks;
}

std::wstring AudioTime::ToString() const
{
	std::wostringstream oss;
	oss << std::setfill(L'0') << std::setw(2)
		<< m_upper.quot << L':' << m_lower.quot << L',' << std::setw(3) << m_lower.rem
		<< std::setfill(L' ') << std::setw(0) << std::ends;

	return oss.str();
}

int AudioTime::get_Minutes() const
{
	return static_cast<int>(m_upper.quot);
}

int AudioTime::get_Seconds() const
{
	return static_cast<int>(m_lower.quot);
}

int AudioTime::get_Milliseconds() const
{
	return static_cast<int>(m_lower.rem);
}

long long AudioTime::get_Value() const
{
	return m_ticks;
}

void AudioTime::put_Value(long long value)
{
	m_ticks = value;
	long long disp = m_ticks / 1000ll;

	m_upper = std::lldiv(m_ticks, 60000ll);
	m_lower = std::lldiv(m_upper.rem, 1000ll);
}


