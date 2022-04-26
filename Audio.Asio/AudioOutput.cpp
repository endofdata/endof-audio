#include "pch.h"
#include "AudioOutput.h"
#include <SampleConversionUnmanaged.h>
#include <ObjectFactory.h>

using namespace System;
using namespace Audio::Asio;
using namespace Audio::Asio::Interop;
using namespace Audio::Foundation;
using namespace Audio::Foundation::Abstractions;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

AudioOutput::AudioOutput(int sampleRate, int sampleCount, IOutputChannelPair* pHwChannel, int id) : m_isDisposed(false)
{
	if(NULL == pHwChannel)
		throw gcnew ArgumentNullException();

	m_channelId = id;

	m_pOutputMeter = ObjectFactory::CreateMeterChannel(sampleCount);
	m_pOutputMeter->RMSTime = 100;

	pHwChannel->AddRef();
	m_pOutputChannelPair = pHwChannel;

	m_pMasterMix = ObjectFactory::CreateSampleJoiner(sampleCount);

	ISampleContainer* pSampleContainer;
	m_pMasterMix->QueryInterface(_uuidof(ISampleContainer), (void**)&pSampleContainer);

	ISampleReceiver* pSampleReceiver;
	m_pOutputChannelPair->QueryInterface(_uuidof(ISampleReceiver), (void**)&pSampleReceiver);

	m_pMasterMix->OutputLink = ObjectFactory::CreateChannelLink(pSampleContainer, pSampleReceiver, VolumeMax, PanCenter);

	pSampleReceiver->Release();
	pSampleContainer->Release();
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
		if (NULL != m_pMasterMix)
		{
			m_pMasterMix->Release();
			m_pMasterMix = NULL;
		}
		if (NULL != m_pOutputChannelPair)
		{
			m_pOutputChannelPair->Release();
			m_pOutputChannelPair = NULL;
		}
	}
}

int AudioOutput::ChannelId::get()
{
	return m_channelId;
}

Level AudioOutput::DbFS::get()
{
	return Level(m_pOutputMeter->DbLeft, m_pOutputMeter->DbRight);
}

IOutputChannelPair& AudioOutput::OutputChannelPair::get()
{
	return *m_pOutputChannelPair;
}

void AudioOutput::WriteCurrentFrame(array<float>^ frameBuffer, float volume, float pan)
{
	if(nullptr != frameBuffer)
	{
		pin_ptr<float> pBuffer = &frameBuffer[0];
		m_pMasterMix->MixInput(pBuffer, pBuffer, volume, pan);
	}
}

void AudioOutput::Send()
{
	m_pMasterMix->Send();
	m_pMasterMix->Flush();
}

void AudioOutput::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

