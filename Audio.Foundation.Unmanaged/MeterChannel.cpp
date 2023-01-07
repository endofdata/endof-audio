#include "pch.h"
#include "MeterChannel.h"
#include "SampleConversionUnmanaged.h"
#include "ISampleContainer.h"
#include "ISampleBuffer.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

MeterChannel::MeterChannel(int sampleRate) :
	m_sampleRate(sampleRate),
	m_iSamplesPerRMSFrame(880), // ~20 ms @ 44.1 kHz
	m_meterUpdate(NULL),
	m_pWriteThrough(NULL),
	m_refCount(0)
{
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
	m_dSumUpLeft = 0.0f;
	m_dSumUpRight = 0.0f;
	m_fDbLeft = -1000.0f;
	m_fDbRight = -1000.0f;
}

void MeterChannel::Receive(ISampleContainer& input)
{
	if (m_pWriteThrough != NULL)
	{
		m_pWriteThrough->Receive(input);
	}

	ISampleBuffer* pLeftChannel = input.LeftChannel;
	ISampleBuffer* pRightChannel = input.RightChannel;

	int sumUp = 0;

	if (pLeftChannel != NULL)
	{
		SampleConversion::ContinueRMSSumUp(pLeftChannel->SamplePtr, pLeftChannel->SampleCount, m_dSumUpLeft);
		sumUp = pLeftChannel->SampleCount;
	}
	if (pRightChannel != NULL)
	{
		SampleConversion::ContinueRMSSumUp(pRightChannel->SamplePtr, pRightChannel->SampleCount, m_dSumUpRight);
		sumUp = pRightChannel->SampleCount;
	}
	if (sumUp > 0)
	{
		m_iSumUpSamples += pLeftChannel->SampleCount;
	}
	if (m_iSumUpSamples >= m_iSamplesPerRMSFrame)
	{
		m_fDbLeft = (float)SampleConversion::DbFullScaleRMS(m_dSumUpLeft, m_iSumUpSamples);
		m_fDbRight = (float)SampleConversion::DbFullScaleRMS(m_dSumUpRight, m_iSumUpSamples);
		m_dSumUpLeft = 0.0f;
		m_dSumUpRight = 0.0f;
		m_iSumUpSamples = 0;

		OnMeterUpdate();
	}
}

float MeterChannel::get_DbLeft()
{
	return m_fDbLeft;
}

float MeterChannel::get_DbRight()
{
	return m_fDbRight;
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