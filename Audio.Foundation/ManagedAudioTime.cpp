#include "pch.h"
#include "ManagedAudioTime.h"

using namespace Audio::Foundation::Interop;

ManagedAudioTime::ManagedAudioTime()
{
	_ticks = 0;
}

ManagedAudioTime::ManagedAudioTime(long long micros)
{
	_ticks = micros;
}

ManagedAudioTime::ManagedAudioTime(ManagedAudioTime^ other)
{
	_ticks = other->Value;
}

ManagedAudioTime::ManagedAudioTime(const AudioTime& audioTime)
{
	_ticks = audioTime;
}

ManagedAudioTime::~ManagedAudioTime()
{
}

int ManagedAudioTime::Minutes::get()
{
	return static_cast<int>(_upper.Item1);
}

int ManagedAudioTime::Seconds::get()
{
	return static_cast<int>(_lower.Item1);
}

int ManagedAudioTime::MilliSeconds::get()
{
	return static_cast<int>(_lower.Item2);
}

long long ManagedAudioTime::Value::get()
{
	return _ticks;
}

void ManagedAudioTime::Value::set(long long value)
{
	_ticks = value;
	long long disp = _ticks / 1000ll;

	_upper = System::Math::DivRem(disp, 60000ll);
	_lower = System::Math::DivRem(_upper.Item2, 1000ll);
}
