#include "pch.h"
#include "ManagedAudioTime.h"

using namespace Audio::Foundation::Interop;

ManagedAudioTime::ManagedAudioTime()
{
	_micros = 0;
}

ManagedAudioTime::ManagedAudioTime(long long micros)
{
	_micros = micros;
}

ManagedAudioTime::ManagedAudioTime(ManagedAudioTime^ other)
{
	_micros = other->Value;
}

ManagedAudioTime::ManagedAudioTime(const AudioTime& audioTime)
{
	_micros = audioTime;
}

ManagedAudioTime::~ManagedAudioTime()
{
}

int ManagedAudioTime::Minutes::get()
{
	return AudioTime(_micros).Minutes;
}

int ManagedAudioTime::Seconds::get()
{
	return AudioTime(_micros).Seconds;
}

int ManagedAudioTime::MilliSeconds::get()
{
	return AudioTime(_micros).Milliseconds;
}

long long ManagedAudioTime::Value::get()
{
	return _micros;
}

void ManagedAudioTime::Value::set(long long value)
{
	_micros = value;
}
