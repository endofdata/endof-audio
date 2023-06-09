#include "pch.h"
#include "AudioOutputBase.h"

using namespace Audio::Foundation::Interop;

AudioOutputBase::AudioOutputBase(int channelId)
{
	m_channelId = channelId;
}

AudioOutputBase::~AudioOutputBase()
{
}

int AudioOutputBase::ChannelId::get()
{
	return m_channelId;
}

void AudioOutputBase::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

