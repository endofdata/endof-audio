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

AudioOutput::AudioOutput(int sampleRate, int sampleCount, IOutputChannelPair* pHwChannel, int id) : 
	AudioOutputBase(id),
	m_isDisposed(false),
	m_pOutputChannelPair(NULL),
	m_pOutputJoinerReceiver(NULL),
	m_pOutputMeter(NULL)
{
	if(NULL == pHwChannel)
		throw gcnew ArgumentNullException();

	pHwChannel->AddRef();
	m_pOutputChannelPair = pHwChannel;

	// Create output meter including meter update event handler
	m_pOutputMeter = ObjectFactory::CreateMeterChannel(sampleCount, 2);
	m_pOutputMeter->RMSTime = 100;
	MeterUpdateDelegate^ meterUpdateDelegate = gcnew MeterUpdateDelegate(this, &AudioOutput::OutputMeter_MeterUpdate);
	m_meterUpdateDelegateHandle = GCHandle::Alloc(meterUpdateDelegate);
	m_pOutputMeter->MeterUpdate = static_cast<MeterChannelCallback>(Marshal::GetFunctionPointerForDelegate(meterUpdateDelegate).ToPointer());

	// Chain the output-channel receiver to the write-through of the meter channel
	ISampleReceiver* pOutputSampleReceiver = NULL;
	m_pOutputChannelPair->QueryInterface(_uuidof(ISampleReceiver), (void**)&pOutputSampleReceiver);
	m_pOutputMeter->WriteThrough = pOutputSampleReceiver;
	pOutputSampleReceiver->Release();

	// Chain the meter-channel receiver to the master mix
	ISampleReceiver* pMeterSampleReceiver = NULL;
	m_pOutputMeter->QueryInterface(__uuidof(ISampleReceiver), (void**)&pMeterSampleReceiver);
	m_pOutputChannelPair->SampleJoiner.Target = pMeterSampleReceiver;
	pMeterSampleReceiver->Release();

	// Store the ISampleReceiver iface of the output's sample joiner
	ISampleReceiver* pOutputJoinerReceiver = NULL;
	m_pOutputChannelPair->SampleJoiner.QueryInterface(__uuidof(ISampleReceiver), (void**)&pOutputJoinerReceiver);
	m_pOutputJoinerReceiver = pOutputJoinerReceiver;
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

		if (NULL != m_pOutputJoinerReceiver)
		{
			m_pOutputJoinerReceiver->Release();
			m_pOutputJoinerReceiver = NULL;
		}
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

IOutputChannelPair& AudioOutput::OutputChannelPair::get()
{
	return *m_pOutputChannelPair;
}

void AudioOutput::Send()
{
	m_pOutputJoinerReceiver->Flush();
}

void AudioOutput::OutputMeter_MeterUpdate(IntPtr sender)
{
	OnPropertyChanged(DbFSProperty);
}