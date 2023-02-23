#include "pch.h"
#include "AudioTime.h"
#include <algorithm>
#include <string>
#include <sstream>

using namespace Audio::Foundation::Unmanaged;

AudioTime AudioTime::FromSeconds(double seconds)
{
	return AudioTime(static_cast<long long>(seconds * 1000.0));
}

AudioTime::AudioTime()
{
	value = 0;
}

AudioTime::AudioTime(long long value)
{
	this->value = value;
}

AudioTime::operator long long() const
{
	return value;
}

AudioTime& AudioTime::operator + (const AudioTime& other)
{
	value += other.value;
	return *this;
}

AudioTime& AudioTime::operator - (const AudioTime& other)
{
	value += other.value;
	return *this;
}

AudioTime& AudioTime::operator += (const AudioTime& other)
{
	value += other.value;
	return *this;
}

AudioTime& AudioTime::operator -= (const AudioTime& other)
{
	value -= other.value;
	return *this;
}

bool AudioTime::operator == (const AudioTime& other) const
{
	return value == other.value;
}

bool AudioTime::operator != (const AudioTime& other) const
{
	return value != other.value;
}

bool AudioTime::operator > (const AudioTime& other) const
{
	return value > other.value;
}

bool AudioTime::operator < (const AudioTime& other) const
{
	return value < other.value;
}

bool AudioTime::operator >= (const AudioTime& other) const
{
	return value >= other.value;
}

bool AudioTime::operator <= (const AudioTime& other) const
{
	return value <= other.value;
}

std::wstring AudioTime::ToString() const
{
	auto minutes = std::lldiv(value, 60000);
	auto seconds = std::lldiv(minutes.rem, 1000);
	std::wostringstream oss;
	oss << minutes.quot << L':' << seconds.quot << L',' << seconds.rem;

	return oss.str();
}

int AudioTime::get_Minutes() const
{
	return static_cast<int>(std::lldiv(value, 60000).quot);
}

int AudioTime::get_Seconds() const
{
	return static_cast<int>(std::lldiv(std::lldiv(value, 60000).rem, 1000).quot);
}

int AudioTime::get_Milliseconds() const
{
	return static_cast<int>(std::lldiv(std::lldiv(value, 60000).rem, 1000).rem);
}

long long AudioTime::get_Value() const
{
	return value;
}


