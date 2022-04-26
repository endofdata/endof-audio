#include "pch.h"
#include "AudioOutputBase.h"

using namespace Audio::Foundation::Interop;

AudioOutputBase::AudioOutputBase(int channelId)
{
	m_channelId = channelId;
}

// virtual 
AudioOutputBase::~AudioOutputBase()
{
}

// virtual 
int AudioOutputBase::ChannelId::get()
{
	return m_channelId;
}

// virtual 
void AudioOutputBase::ChannelId::set(int value)
{
	m_channelId = value;
}


// virtual 
Level AudioOutputBase::DbFS::get()
{
	return m_dbFS;
}

// virtual 
void AudioOutputBase::DbFS::set(Level value)
{
	m_dbFS = value;
}
