#include "pch.h"
#include "MeterChannel.h"
#include "SampleConversionUnmanaged.h"
#include "ISampleContainer.h"
#include "ISampleBuffer.h"
#include <algorithm>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

const float MeterChannel::DbFSMin = -1000.0f;

MeterChannel::MeterChannel(int sampleRate, int channelCount) :
	m_sampleRate(sampleRate),
	m_iSamplesPerRMSFrame(880), // ~20 ms @ 44.1 kHz
	m_meterUpdate(nullptr),
	m_pWriteThrough(nullptr),
	m_refCount(0)
{
	m_vecSumUp.resize(channelCount, 0.0);
	m_vecDbFS.resize(channelCount, DbFSMin);
	Flush();
}

MeterChannel::~MeterChannel()
{
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

	*ppvResult = nullptr;
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
		value = DbFSMin;
	});
}

void MeterChannel::Receive(ISampleContainerPtr input)
{
	if (m_pWriteThrough != nullptr)
	{
		m_pWriteThrough->Receive(input);
	}

	int maxChannels = min(input->ChannelCount, (int)m_vecSumUp.size());

	if (maxChannels > 0)
	{
		for (int c = 0; c < maxChannels; c++)
		{
			ISampleBuffer* pChannel = input->Channels[c];
			SampleConversion::ContinueRMSSumUp(pChannel->SamplePtr, pChannel->SampleCount, m_vecSumUp.at(c));
		}

		m_iSumUpSamples += input->SampleCount;

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
	return (int)m_vecDbFS.size();
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

ISampleReceiverPtr MeterChannel::get_WriteThrough()
{
	return m_pWriteThrough;
}

void MeterChannel::put_WriteThrough(ISampleReceiverPtr value)
{
	m_pWriteThrough = value;
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

	if (nullptr != handler)
		handler(this);
}
