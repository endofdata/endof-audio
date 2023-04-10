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

AudioTime::AudioTime()
{
	m_micros = 0;
}

AudioTime::AudioTime(const std::chrono::microseconds& micros)
{
	m_micros = micros.count();
}

AudioTime::AudioTime(const long long value)
{
	m_micros = value;
}

AudioTime::operator long long() const
{
	return m_micros;
}

AudioTime& AudioTime::operator + (const AudioTime& other)
{
	m_micros += other.m_micros;
	return *this;
}

AudioTime& AudioTime::operator - (const AudioTime& other)
{
	m_micros += other.m_micros;
	return *this;
}

AudioTime& AudioTime::operator += (const AudioTime& other)
{
	m_micros += other.m_micros;
	return *this;
}

AudioTime& AudioTime::operator -= (const AudioTime& other)
{
	m_micros -= other.m_micros;
	return *this;
}

bool AudioTime::operator == (const AudioTime& other) const
{
	return m_micros == other.m_micros;
}

bool AudioTime::operator != (const AudioTime& other) const
{
	return m_micros != other.m_micros;
}

bool AudioTime::operator > (const AudioTime& other) const
{
	return m_micros > other.m_micros;
}

bool AudioTime::operator < (const AudioTime& other) const
{
	return m_micros < other.m_micros;
}

bool AudioTime::operator >= (const AudioTime& other) const
{
	return m_micros >= other.m_micros;
}

bool AudioTime::operator <= (const AudioTime& other) const
{
	return m_micros <= other.m_micros;
}

std::wstring AudioTime::ToString() const
{
	auto minutes = std::lldiv(m_micros, 60000000);
	auto seconds = std::lldiv(minutes.rem, 1000000);
	std::wostringstream oss;
	oss << std::setfill(L'0') << std::setw(2)
		<< minutes.quot << L':' << seconds.quot << L',' << std::setw(3) << seconds.rem
		<< std::setfill(L' ') << std::setw(0) << std::ends;

	return oss.str();
}

int AudioTime::get_Minutes() const
{
	return static_cast<int>(std::lldiv(m_micros, 60000000).quot);
}

int AudioTime::get_Seconds() const
{
	return static_cast<int>(std::lldiv(std::lldiv(m_micros, 60000000).rem, 1000).quot);
}

int AudioTime::get_Milliseconds() const
{
	return static_cast<int>(std::lldiv(std::lldiv(m_micros, 60000000).rem, 1000).rem);
}

long long AudioTime::get_Value() const
{
	return m_micros;
}

void AudioTime::put_Value(long long value)
{
	m_micros = value;
}


