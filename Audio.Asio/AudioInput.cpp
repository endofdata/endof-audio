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
using namespace Audio::Foundation::Interop;
using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

AudioInput::AudioInput(int sampleRate, IInputChannel* pHwChannel, int id) : 
	AudioInputBase(id),
	m_isDisposed(false)
{
	if(NULL == pHwChannel)
		throw gcnew ArgumentNullException();

	pHwChannel->AddRef();
	m_pInputChannel = pHwChannel;

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

		RemoveAllTargets();

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

Level AudioInput::DbFS::get()
{
	// We have only a single channel, so use index 0 twice
	return Level(m_pInputMeter->DbFS[0], m_pInputMeter->DbFS[0]);
}

bool AudioInput::OnSetMonitor(IAudioOutput^ value)
{
	if (nullptr != value)
	{
		AudioOutput^ output = safe_cast<AudioOutput^>(value);

		if (output == nullptr)
		{
			m_pInputChannel->DirectMonitor = NULL;
			return false;
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
	AudioInputBase::Monitor = value;

	return true;
}

bool AudioInput::OnAddTarget(IAudioOutput^ target)
{
	AudioOutput^ output = safe_cast<AudioOutput^>(target);

	if (output != nullptr)
	{
		ISampleReceiver* pJoinerAsReceiver = NULL;

		if (SUCCEEDED(output->OutputChannelPair.SampleJoiner.QueryInterface(__uuidof(ISampleReceiver), (void**)&pJoinerAsReceiver)))
		{
			m_pInputChannel->SampleSharer.AddTarget(*pJoinerAsReceiver);
			pJoinerAsReceiver->Release();
			return true;
		}
	}
	return false;
}

void AudioInput::OnRemoveTarget(IAudioOutput^ target)
{
	AudioOutput^ output = safe_cast<AudioOutput^>(target);

	ISampleReceiver* pJoinerAsReceiver = NULL;

	if (SUCCEEDED(output->OutputChannelPair.SampleJoiner.QueryInterface(__uuidof(ISampleReceiver), (void**)&pJoinerAsReceiver)))
	{
		m_pInputChannel->SampleSharer.RemoveTarget(*pJoinerAsReceiver);
		pJoinerAsReceiver->Release();
	}
}

void AudioInput::InputMeter_MeterUpdate(IntPtr sender)
{
	OnPropertyChanged(DbFSProperty);
}
