#include "pch.h"
#include "AudioInputBase.h"

using namespace Audio::Foundation::Interop;
using namespace System::Collections::Generic;

AudioInputBase::AudioInputBase(int channelId)
{
	m_channelId = channelId;
}

AudioInputBase::~AudioInputBase()
{
}

int AudioInputBase::ChannelId::get()
{
	return m_channelId;
}

void AudioInputBase::ChannelId::set(int value)
{
	m_channelId = value;

	OnPropertyChanged(ChannelIdProperty);
}

IAudioOutput^ AudioInputBase::Monitor::get()
{
	return m_monitor;
}

void AudioInputBase::Monitor::set(IAudioOutput^ value)
{
	if (OnSetMonitor(value))
	{
		m_monitor = value;
		OnPropertyChanged(MonitorProperty);
	}
}
