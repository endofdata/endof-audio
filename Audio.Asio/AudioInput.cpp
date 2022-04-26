#include "pch.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include <vcclr.h>
#include <ISampleSharer.h>
#include <ObjectFactory.h>
#include <SampleConversionUnmanaged.h>

using namespace System;
using namespace System::Runtime::InteropServices;
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

	m_channelId = id;

	MeterUpdateDelegate^ meterUpdateDelegate = gcnew MeterUpdateDelegate(this, &AudioInput::InputMeter_MeterUpdate);
	m_meterUpdateDelegateHandle = GCHandle::Alloc(meterUpdateDelegate);

	m_pInputMeter = Audio::Foundation::Unmanaged::ObjectFactory::CreateMeterChannel(sampleRate);
	m_pInputMeter->RMSTime = 100;
	m_pInputMeter->MeterUpdate = static_cast<MeterChannelCallback>(Marshal::GetFunctionPointerForDelegate(meterUpdateDelegate).ToPointer());

	ISampleReceiver* pSampleReceiver;

	m_pInputMeter->QueryInterface(__uuidof(ISampleReceiver), (void**)&pSampleReceiver);

	pHwChannel->AddRef();
	m_pInputChannel = pHwChannel;
	m_pInputChannel->SampleSharer.AddSend(m_pInputChannel->SampleContainer, *pSampleReceiver, VolumeMax, PanCenter);

	pSampleReceiver->Release();
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
}

int AudioInput::ChannelId::get()
{
	return m_channelId;
}

Level AudioInput::DbFS::get()
{
	return Level(m_pInputMeter->DbLeft, m_pInputMeter->DbRight);
}

bool AudioInput::IsActive::get()
{
	return m_pInputChannel->SampleContainer.IsActive;
}

void AudioInput::IsActive::set(bool value)
{
	m_pInputChannel->SampleContainer.IsActive = value;

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
		m_pInputChannel->Monitor = &output->OutputChannelPair;
	}
	else
		m_pInputChannel->Monitor = NULL;
	m_monitor = value;

	OnPropertyChanged(MonitorProperty);
}

void AudioInput::ReadCurrentFrame(array<float>^ frameBuffer)
{
	System::Runtime::InteropServices::Marshal::Copy(System::IntPtr((void*)m_pInputChannel->SampleContainer.LeftChannel->SamplePtr), frameBuffer, 0, frameBuffer->Length);
}

void AudioInput::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

void AudioInput::InputMeter_MeterUpdate(IntPtr sender)
{
	OnPropertyChanged(DbFSProperty);
}
