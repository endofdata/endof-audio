#include "pch.h"
#include "AudioTakeBase.h"

using namespace System;
using namespace System::IO;
using namespace Audio::Foundation;
using namespace Audio::Foundation::Interop;

AudioTakeBase::AudioTakeBase(TimeSpan offset, int sampleRate)
{
	m_offset = offset;
	m_sampleRate = sampleRate;
}

AudioTakeBase::~AudioTakeBase()
{
}

float AudioTakeBase::Volume::get()
{
	return m_volume;
}

void AudioTakeBase::Volume::set(float value)
{
	m_volume = max(0.0f, min(value, 1.0f));
}

TimeSpan AudioTakeBase::Offset::get()
{
	return m_offset;
}

void AudioTakeBase::Offset::set(TimeSpan value)
{
	m_offset = value;
}

TimeSpan AudioTakeBase::End::get()
{
	return TimeSpan::operator+(Offset, Length);
}

int AudioTakeBase::SampleRate::get()
{
	return m_sampleRate;
}

TimeSpan AudioTakeBase::Position::get()
{
	throw gcnew NotImplementedException();
}

void AudioTakeBase::Position::set(TimeSpan value)
{
	throw gcnew NotImplementedException();
}

TimeSpan AudioTakeBase::Length::get()
{
	throw gcnew NotImplementedException();
}

bool AudioTakeBase::TrySetPosition(TimeSpan position)
{
	bool result;

	if (Offset > position)
	{
		Position = Offset;
		result = false;
	}
	else
	{
		Position = position;
		result = true;
	}
	return result;
}

void AudioTakeBase::Initialize()
{
}

int AudioTakeBase::ReadNextFrame(cli::array<float>^ audioBuffer)
{
	throw gcnew NotImplementedException();
}
