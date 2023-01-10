#include "pch.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include <SampleConversionUnmanaged.h>
#include <ObjectFactory.h>

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace Audio::Asio;
using namespace Audio::Asio::Interop;
using namespace Audio::Foundation;
using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

AudioOutput::AudioOutput(int sampleRate, int sampleCount, IOutputChannelPairPtr pHwChannel, int id) : 
	AudioOutputBase(id),
	m_isDisposed(false),
	m_pOutputChannelPair(NULL),
	m_pOutputMeter(NULL)
{
	if(NULL == pHwChannel)
		throw gcnew ArgumentNullException();

	m_pOutputChannelPair = pHwChannel.Detach();

	// Create output meter including meter update event handler
	m_pOutputMeter = ObjectFactory::CreateMeterChannel(sampleRate, 2).Detach();
	m_pOutputMeter->RMSTime = 100;
	MeterUpdateDelegate^ meterUpdateDelegate = gcnew MeterUpdateDelegate(this, &AudioOutput::OutputMeter_MeterUpdate);
	m_meterUpdateDelegateHandle = GCHandle::Alloc(meterUpdateDelegate);
	m_pOutputMeter->MeterUpdate = static_cast<MeterChannelCallback>(Marshal::GetFunctionPointerForDelegate(meterUpdateDelegate).ToPointer());

	// Chain the output-channel receiver to the write-through of the meter channel
	m_pOutputMeter->WriteThrough = m_pOutputChannelPair;

	// Chain the meter-channel receiver to the master mix
	m_pOutputChannelPair->SampleJoiner->Target = m_pOutputMeter;
}

AudioOutput::~AudioOutput()
{
	CleanUp(true);
}

AudioOutput::!AudioOutput()
{
	CleanUp(false);
}

void AudioOutput::CleanUp(bool isDisposing)
{
	if (!m_isDisposed)
	{
		m_isDisposed = true;

		if (NULL != m_pOutputMeter)
		{
			m_pOutputMeter->Release();
			m_pOutputMeter = NULL;
		}
		if (NULL != m_pOutputChannelPair)
		{
			m_pOutputChannelPair->Release();
			m_pOutputChannelPair = NULL;
		}

		m_meterUpdateDelegateHandle.Free();
	}
}

Level AudioOutput::DbFS::get()
{
	return Level(m_pOutputMeter->DbFS[0], m_pOutputMeter->DbFS[1]);
}

IOutputChannelPairPtr AudioOutput::OutputChannelPair::get()
{
	return m_pOutputChannelPair;
}

void AudioOutput::Send()
{
	ISampleReceiverPtr pReceiver = m_pOutputChannelPair->SampleJoiner;
	pReceiver->Flush();
}

void AudioOutput::OutputMeter_MeterUpdate(IntPtr sender)
{
	OnPropertyChanged(DbFSProperty);
}