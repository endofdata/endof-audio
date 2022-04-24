#include "pch.h"
#include "MeterChannel.h"
#include "SampleConversionUnmanaged.h"
#include "ISampleContainer.h"
#include "ISampleBuffer.h"
#include "IChannelLink.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

MeterChannel::MeterChannel(int sampleRate) :
	m_sampleRate(sampleRate),
	m_iSamplesPerRMSFrame(880), // ~20 ms @ 44.1 kHz
	m_iSumUpSamples(0),
	m_dSumUpLeft(0.0f),
	m_dSumUpRight(0.0f),
	m_fDbLeft(0.0f),
	m_fDbRight(0.0f),
	m_pInput(NULL),
	m_meterUpdate(NULL)
{
}

void MeterChannel::Flush()
{
}

void MeterChannel::Receive(IChannelLink& inputBuffer)
{
	ISampleContainer* pInput = inputBuffer.Input;

	if(NULL != pInput)
	{
		ISampleBuffer* pLeftChannel = pInput->LeftChannel;
		ISampleBuffer* pRightChannel = pInput->RightChannel;

		SampleConversion::ContinueRMSSumUp(pLeftChannel->SamplePtr, pLeftChannel->SampleCount, m_dSumUpLeft);
		SampleConversion::ContinueRMSSumUp(pRightChannel->SamplePtr, pRightChannel->SampleCount, m_dSumUpRight);
		m_iSumUpSamples += pLeftChannel->SampleCount;
		if(m_iSumUpSamples >= m_iSamplesPerRMSFrame)
		{
			m_fDbLeft = (float)SampleConversion::DbFullScaleRMS(m_dSumUpLeft, m_iSumUpSamples);
			m_fDbRight = (float)SampleConversion::DbFullScaleRMS(m_dSumUpRight, m_iSumUpSamples);
			m_dSumUpLeft = 0.0f;
			m_dSumUpRight = 0.0f;
			m_iSumUpSamples = 0;

			OnMeterUpdate();
		}
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

IChannelLink* MeterChannel::get_Input()
{
	return m_pInput;
}

void MeterChannel::put_Input(IChannelLink* value)
{
	m_pInput = value;
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

	if(NULL != handler)
		handler(this);
}