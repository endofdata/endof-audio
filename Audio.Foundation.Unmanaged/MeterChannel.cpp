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
	m_isBypassed(false),
	m_refCount(0)
{
	m_vecSumUp.resize(channelCount, 0.0);
	m_vecDbFS.resize(channelCount, DbFSMin);
	Reset();
}

MeterChannel::~MeterChannel()
{
}

IMPLEMENT_IUNKNOWN(MeterChannel)

void* MeterChannel::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(dynamic_cast<IMeterChannel*>(this));
	}

	if (iid == __uuidof(IMeterChannel))
	{
		return dynamic_cast<IMeterChannel*>(this);
	}

	if (iid == __uuidof(ISampleProcessor))
	{
		return dynamic_cast<ISampleProcessor*>(this);
	}
	return nullptr;
}

void MeterChannel::Reset()
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

int MeterChannel::Process(ISampleContainerPtr& input, const ProcessingContext& context)
{
	int maxChannels = std::min(input->ChannelCount, (int)m_vecSumUp.size());

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

			Reset();

			OnMeterUpdate();
		}
	}
	return input->SampleCount;
}

bool MeterChannel::get_IsBypassed() const
{
	return m_isBypassed;
}

void MeterChannel::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}