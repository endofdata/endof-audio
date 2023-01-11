#include "pch.h"
#include "AudioSourceBase.h"

using namespace Audio::Foundation::Interop;
using namespace System::Collections::Generic;

AudioSourceBase::AudioSourceBase()
{
	m_targets = gcnew List<IAudioTarget^>();
}

AudioSourceBase::~AudioSourceBase()
{
}

System::Boolean AudioSourceBase::IsActive::get()
{
	return m_isActive;
}

void AudioSourceBase::IsActive::set(System::Boolean value)
{
	m_isActive = value;
	OnPropertyChanged(IsActiveProperty);
}


IEnumerable<IAudioTarget^>^ AudioSourceBase::Targets::get()
{
	return m_targets;
}

bool AudioSourceBase::AddTarget(IAudioTarget^ target)
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

bool AudioSourceBase::RemoveTarget(IAudioTarget^ target)
{
	if (m_targets->Remove(target))
	{
		OnRemoveTarget(target);
		OnPropertyChanged(TargetsProperty);

		return true;
	}
	return false;
}

void AudioSourceBase::RemoveAllTargets()
{
	for each (IAudioTarget ^ target in m_targets)
	{
		OnRemoveTarget(target);
	}
	OnPropertyChanged(TargetsProperty);
}

bool AudioSourceBase::OnAddTarget(IAudioTarget^ target)
{
	return true;
}

void AudioSourceBase::OnRemoveTarget(IAudioTarget^ target)
{
}

void AudioSourceBase::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}