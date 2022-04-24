#include "pch.h"
#include "AudioOutput.h"
#include <SampleConversion.h>
#include <ISampleLinker.h>
#include <ChannelLink.h>

using namespace System;
using namespace Audio::Asio;
using namespace Audio::Asio::Interop;
using namespace Audio::Foundation;
using namespace Audio::Foundation::Abstractions;

AudioOutput::AudioOutput(int sampleRate, int sampleCount, IOutputChannelPair* pHwChannel, int id)
{
	if(NULL == pHwChannel)
		throw gcnew ArgumentNullException();

	m_channelId = id;

	m_pOutputMeter = new MeterChannel(sampleRate);
	m_pOutputMeter->RMSTime = 100;

	m_pOutputChannelPair = pHwChannel;

	m_pMasterMix = new SampleJoiner(sampleCount);
	m_pMasterMix->OutputLink = new ChannelLink(dynamic_cast<SampleContainer*>(m_pMasterMix), &m_pOutputChannelPair->AsSampleReceiver, VolumeMax, PanCenter);
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
	if(NULL != m_pOutputMeter)
	{
		delete m_pOutputMeter;
		m_pOutputMeter = NULL;
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

