#include "pch.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include <vcclr.h>
#include <SampleConversion.h>
#include <ISampleSharer.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Audio::Asio;
using namespace Audio::Asio::Interop;
using namespace Audio::Foundation;
using namespace Audio::Foundation::Abstractions;

AudioInput::AudioInput(int sampleRate, IInputChannel* pHwChannel, int id)
{
	if(NULL == pHwChannel)
		throw gcnew ArgumentNullException();

	m_channelId = id;

	MeterUpdateDelegate^ meterUpdateDelegate = gcnew MeterUpdateDelegate(this, &AudioInput::InputMeter_MeterUpdate);
	m_meterUpdateDelegateHandle = GCHandle::Alloc(meterUpdateDelegate);

	m_pInputMeter = new MeterChannel(sampleRate);
	m_pInputMeter->RMSTime = 100;
	m_pInputMeter->MeterUpdate = static_cast<MeterChannelCallback>(Marshal::GetFunctionPointerForDelegate(meterUpdateDelegate).ToPointer());

	m_pInputChannel = pHwChannel;
	m_pInputChannel->AsSampleSharer.AddSend(m_pInputChannel->AsSampleContainer, *m_pInputMeter, Audio::Foundation::VolumeMax, Audio::Foundation::PanCenter);
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
	if(NULL != m_pInputMeter)
	{
		m_pInputMeter->MeterUpdate = NULL;
		delete m_pInputMeter;
		m_pInputMeter = NULL;
	}
	m_meterUpdateDelegateHandle.Free();
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
	return m_pInputChannel->AsSampleContainer.IsActive;
}

void AudioInput::IsActive::set(bool value)
{
	m_pInputChannel->AsSampleContainer.IsActive = value;

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
	System::Runtime::InteropServices::Marshal::Copy(System::IntPtr((void*)m_pInputChannel->AsSampleContainer.LeftChannel->SamplePtr), frameBuffer, 0, frameBuffer->Length);
}

void AudioInput::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

void AudioInput::InputMeter_MeterUpdate(IntPtr sender)
{
	OnPropertyChanged(DbFSProperty);
}
