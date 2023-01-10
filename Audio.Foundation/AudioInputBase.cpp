#include "pch.h"
#include "AudioInputBase.h"

using namespace Audio::Foundation::Interop;
using namespace System::Collections::Generic;

AudioInputBase::AudioInputBase(int channelId)
{
	m_channelId = channelId;
	m_targets = gcnew List<IAudioOutput^>();
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

System::Boolean AudioInputBase::IsActive::get()
{
	return m_isActive;
}

void AudioInputBase::IsActive::set(System::Boolean value)
{
	m_isActive = value;
	OnPropertyChanged(IsActiveProperty);
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


IEnumerable<IAudioOutput^>^ AudioInputBase::Targets::get()
{
	return m_targets;
}

bool AudioInputBase::AddTarget(IAudioOutput^ target)
{
	if (!m_targets->Contains(target))
	{
		if (OnAddTarget(target))
		{
			m_targets->Add(target);
			OnPropertyChanged(TargetsProperty);

			return true;
		}
	}
	return false;
}

bool AudioInputBase::RemoveTarget(IAudioOutput^ target)
{
	if (m_targets->Remove(target))
	{
		OnRemoveTarget(target);
		OnPropertyChanged(TargetsProperty);

		return true;
	}
	return false;
}

void AudioInputBase::RemoveAllTargets()
{
	for each (IAudioOutput ^ target in m_targets)
	{
		OnRemoveTarget(target);
	}
	OnPropertyChanged(TargetsProperty);
}

void AudioInputBase::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}