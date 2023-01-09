#include "pch.h"
#include "MeterChannel.h"
#include "SampleConversionUnmanaged.h"
#include "ISampleContainer.h"
#include "ISampleBuffer.h"
#include <algorithm>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

MeterChannel::MeterChannel(int sampleRate, int channelCount) :
	m_sampleRate(sampleRate),
	m_iSamplesPerRMSFrame(880), // ~20 ms @ 44.1 kHz
	m_meterUpdate(NULL),
	m_pWriteThrough(NULL),
	m_refCount(0)
{
	m_vecSumUp.resize(channelCount, 0.0);
	m_vecDbFS.resize(channelCount, 0.0f);
	Flush();
}

MeterChannel::~MeterChannel()
{
	put_MeterUpdate(NULL);
	put_WriteThrough(NULL);
}

IMPLEMENT_IUNKNOWN(MeterChannel)

bool MeterChannel::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<IMeterChannel*>(this));
		return true;
	}

	if (iid == __uuidof(IMeterChannel))
	{
		*ppvResult = dynamic_cast<IMeterChannel*>(this);
		return true;
	}

	if (iid == __uuidof(ISampleReceiver))
	{
		*ppvResult = dynamic_cast<ISampleReceiver*>(this);
		return true;
	}

	*ppvResult = NULL;
	return false;
}

void MeterChannel::Flush()
{
	m_iSumUpSamples = 0;
	std::for_each(m_vecSumUp.begin(), m_vecSumUp.end(), [](double& value)
	{
		value = 0.0;
	});

	std::for_each(m_vecDbFS.begin(), m_vecDbFS.end(), [](float& value)
	{
		value = 0.0f;
	});
}

void MeterChannel::Receive(ISampleContainer& input)
{
	if (m_pWriteThrough != NULL)
	{
		m_pWriteThrough->Receive(input);
	}

	int maxChannels = min(input.ChannelCount, m_vecSumUp.size());

	if (maxChannels > 0)
	{
		for (int c = 0; c < maxChannels; c++)
		{
			ISampleBuffer* pChannel = input.Channels[c];
			SampleConversion::ContinueRMSSumUp(pChannel->SamplePtr, pChannel->SampleCount, m_vecSumUp.at(c));
		}

		m_iSumUpSamples += input.SampleCount;

		if (m_iSumUpSamples >= m_iSamplesPerRMSFrame)
		{
			int sumUpSamples = m_iSumUpSamples;
			auto dbFSIterator = m_vecDbFS.begin();

			std::for_each(m_vecSumUp.begin(), m_vecSumUp.end(), [dbFSIterator, sumUpSamples](const double& sumUp)
			{
				*dbFSIterator = (float)SampleConversion::DbFullScaleRMS(sumUp, sumUpSamples);
			});

			Flush();

			OnMeterUpdate();
		}
	}
}

int MeterChannel::get_ChannelCount()
{
	return m_vecDbFS.size();
}

float MeterChannel::get_DbFS(int index)
{
	return m_vecDbFS[index];
}

int MeterChannel::get_RMSTime()
{
	return static_cast<int>(SampleConversion::SamplesToMilliSeconds(m_iSamplesPerRMSFrame, m_sampleRate));
}

void MeterChannel::put_RMSTime(int value)
{
	m_iSamplesPerRMSFrame = static_cast<int>(SampleConversion::MilliSecondsToSamples(value, m_sampleRate));
}

ISampleReceiver* MeterChannel::get_WriteThrough()
{
	return m_pWriteThrough;
}

void MeterChannel::put_WriteThrough(ISampleReceiver* value)
{
	if (value != NULL)
	{
		value->AddRef();
	}

	ISampleReceiver* pWriteThrough = (ISampleReceiver*)InterlockedExchangePointer((void**)&m_pWriteThrough, value);

	if (pWriteThrough != NULL)
	{
		pWriteThrough->Release();
	}
}

MeterChannelCallback MeterChannel::get_MeterUpdate()
{
	return m_meterUpdate;
}

void MeterChannel::put_MeterUpdate(MeterChannelCallback value)
{
	m_meterUpdate = value;
}

void MeterChannel::OnMeterUpdate()
{
	MeterChannelCallback handler = m_meterUpdate;

	if (NULL != handler)
		handler(this);
}