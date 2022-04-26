#include "pch.h"
#include "AudioInputBase.h"

using namespace Audio::Foundation::Interop;

AudioInputBase::AudioInputBase(int channelId)
{
	m_channelId = channelId;
}

// virtual 
AudioInputBase::~AudioInputBase()
{
}

// virtual 
int AudioInputBase::ChannelId::get()
{
	return m_channelId;
}

// virtual 
void AudioInputBase::ChannelId::set(int value)
{
	m_channelId = value;
}


// virtual 
Level AudioInputBase::DbFS::get()
{
	return m_dbFS;
}

// virtual 
void AudioInputBase::DbFS::set(Level value)
{
	m_dbFS = value;
}


// virtual 
System::Boolean AudioInputBase::IsActive::get()
{
	return m_isActive;
}

// virtual 
void AudioInputBase::IsActive::set(System::Boolean value)
{
	m_isActive = value;
}


// virtual 
IAudioOutput^ AudioInputBase::Monitor::get()
{
	return m_monitor;
}

// virtual 
void AudioInputBase::Monitor::set(IAudioOutput^ value)
{
	m_monitor = value;
}


