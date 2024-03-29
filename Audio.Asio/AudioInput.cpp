#include "pch.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include <vcclr.h>
#include <ISampleSharer.h>
#include <FoundationObjectFactory.h>
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

AudioInput::AudioInput(int sampleRate, IInputChannelPtr pHwChannel, int id) : 
	AudioInputBase(id),
	m_isDisposed(false)
{
	if(nullptr == pHwChannel)
		throw gcnew ArgumentNullException();

	m_pInputChannel = pHwChannel.Detach();

	m_pInputMeter = Audio::Foundation::Unmanaged::FoundationObjectFactory::CreateMeterChannel(sampleRate, 1).Detach();
	m_pInputMeter->RMSTime = 100;
	MeterUpdateDelegate^ meterUpdateDelegate = gcnew MeterUpdateDelegate(this, &AudioInput::InputMeter_MeterUpdate);
	m_meterUpdateDelegateHandle = GCHandle::Alloc(meterUpdateDelegate);
	m_pInputMeter->MeterUpdate = static_cast<MeterChannelCallback>(Marshal::GetFunctionPointerForDelegate(meterUpdateDelegate).ToPointer());
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

		if (m_pInputChannel != nullptr)
		{
			m_pInputChannel->Release();
			m_pInputChannel = nullptr;
		}

		if (m_pInputMeter != nullptr)
		{
			m_pInputMeter->MeterUpdate = nullptr;
			m_pInputMeter->Release();
			m_pInputMeter = nullptr;
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
	IOutputChannelPairPtr noMonitor;

	if (nullptr != value)
	{
		AudioOutput^ output = safe_cast<AudioOutput^>(value);

		if (output == nullptr)
		{
			m_pInputChannel->DirectMonitor = noMonitor;
			return false;
		}
		else
		{
			m_pInputChannel->DirectMonitor = output->OutputChannelPair;
		}
	}
	else
	{
		m_pInputChannel->DirectMonitor = noMonitor;
	}
	AudioInputBase::Monitor = value;

	return true;
}

bool AudioInput::OnAddTarget(IAudioTarget^ target)
{
	AudioOutput^ output = safe_cast<AudioOutput^>(target);

	//if (output != nullptr)
	//{
	//	((ISampleProcessorPtr)m_pInputChannel)->SampleSharer->AddTarget(output->OutputChannelPair->SampleJoiner);
	//	return true;
	//}
	return false;
}

void AudioInput::OnRemoveTarget(IAudioTarget^ target)
{
	AudioOutput^ output = safe_cast<AudioOutput^>(target);

	//((ISampleProcessorPtr)m_pInputChannel)->SampleSharer->RemoveTarget(output->OutputChannelPair->SampleJoiner);
}

void AudioInput::InputMeter_MeterUpdate(IntPtr sender)
{
	OnPropertyChanged(DbFSProperty);
}
