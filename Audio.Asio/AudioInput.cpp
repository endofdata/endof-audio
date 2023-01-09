#include "pch.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include <vcclr.h>
#include <ISampleSharer.h>
#include <ObjectFactory.h>
#include <SampleConversionUnmanaged.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Collections::Generic;
using namespace Audio::Asio;
using namespace Audio::Asio::Interop;
using namespace Audio::Foundation;
using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

AudioInput::AudioInput(int sampleRate, IInputChannel* pHwChannel, int id) : m_isDisposed(false)
{
	if(NULL == pHwChannel)
		throw gcnew ArgumentNullException();

	pHwChannel->AddRef();
	m_pInputChannel = pHwChannel;
	m_channelId = id;

	m_targets = gcnew List<IAudioOutput^>();

	int inputChannels = m_pInputChannel->SampleSharer.Source->ChannelCount;

	m_pInputMeter = Audio::Foundation::Unmanaged::ObjectFactory::CreateMeterChannel(sampleRate, inputChannels);
	m_pInputMeter->RMSTime = 100;
	MeterUpdateDelegate^ meterUpdateDelegate = gcnew MeterUpdateDelegate(this, &AudioInput::InputMeter_MeterUpdate);
	m_meterUpdateDelegateHandle = GCHandle::Alloc(meterUpdateDelegate);
	m_pInputMeter->MeterUpdate = static_cast<MeterChannelCallback>(Marshal::GetFunctionPointerForDelegate(meterUpdateDelegate).ToPointer());

	ISampleReceiver* pMeteringReceiver;
	m_pInputMeter->QueryInterface(__uuidof(ISampleReceiver), (void**)&pMeteringReceiver);
	m_pInputChannel->SampleSharer.AddTarget(*pMeteringReceiver);
	pMeteringReceiver->Release();
}

AudioInput::~AudioInput()
{
	CleanUp(true);
}

AudioInput::!AudioInput()
{
	CleanUp(false);
}

void AudioInput::CleanUp(bool isDisposing)
{
	if (!m_isDisposed)
	{
		m_isDisposed = true;

		if (m_pInputChannel != NULL)
		{
			m_pInputChannel->Release();
		}

		if (m_pInputMeter != NULL)
		{
			m_pInputMeter->MeterUpdate = NULL;
			m_pInputMeter->Release();
			m_pInputMeter = NULL;
		}

		m_meterUpdateDelegateHandle.Free();
	}
		RemoveAllTargets();
}

int AudioInput::ChannelId::get()
{
	return m_channelId;
}

Level AudioInput::DbFS::get()
{
	// We have only a single channel, so use index 0 twice
	return Level(m_pInputMeter->DbFS[0], m_pInputMeter->DbFS[0]);
}

bool AudioInput::IsActive::get()
{
	return m_pInputChannel->IsActive;
}

void AudioInput::IsActive::set(bool value)
{
	m_pInputChannel->IsActive = value;

	OnPropertyChanged(IsActiveProperty);
}

IAudioOutput^ AudioInput::Monitor::get()
{
	return m_monitor;
}

void AudioInput::Monitor::set(IAudioOutput^ value)
{
	if (nullptr != value)
	{
		AudioOutput^ output = safe_cast<AudioOutput^>(value);

		if (output == nullptr)
		{
			m_pInputChannel->DirectMonitor = NULL;
		}
		else
		{
			m_pInputChannel->DirectMonitor = &output->OutputChannelPair;
		}
	}
	else
	{
		m_pInputChannel->DirectMonitor = NULL;
	}
	m_monitor = value;

	OnPropertyChanged(MonitorProperty);
}

// virtual 
bool AudioInput::AddTarget(IAudioOutput^ target)
{
	if (!m_targets->Contains(target))
	{
		AudioOutput^ output = safe_cast<AudioOutput^>(target);

		if (output != nullptr)
		{
			ISampleReceiver* pJoinerAsReceiver = NULL;

			if (SUCCEEDED(output->OutputChannelPair.SampleJoiner.QueryInterface(__uuidof(ISampleReceiver), (void**)&pJoinerAsReceiver)))
			{
				m_pInputChannel->SampleSharer.AddTarget(*pJoinerAsReceiver);
				m_targets->Add(target);
				pJoinerAsReceiver->Release();
				return true;
			}
		}
	}
	return false;
}

// virtual 
bool AudioInput::RemoveTarget(IAudioOutput^ target)
{
	if (m_targets->Remove(target))
	{
		UnlinkTarget(target);
		return true;
	}
	return false;
}

// virtual
void AudioInput::RemoveAllTargets()
{
	for each (IAudioOutput ^ target in m_targets)
	{
		UnlinkTarget(target);
	}
}

void AudioInput::UnlinkTarget(IAudioOutput^ target)
{
	AudioOutput^ output = safe_cast<AudioOutput^>(target);

	ISampleReceiver* pJoinerAsReceiver = NULL;

	if (SUCCEEDED(output->OutputChannelPair.SampleJoiner.QueryInterface(__uuidof(ISampleReceiver), (void**)&pJoinerAsReceiver)))
	{
		m_pInputChannel->SampleSharer.RemoveTarget(*pJoinerAsReceiver);
		pJoinerAsReceiver->Release();
	}
}

void AudioInput::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

void AudioInput::InputMeter_MeterUpdate(IntPtr sender)
{
	OnPropertyChanged(DbFSProperty);
}
