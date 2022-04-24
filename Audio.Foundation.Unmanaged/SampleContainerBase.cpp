#include "pch.h"
#include "SampleContainerBase.h"
#include "SampleBuffer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainerBase::SampleContainerBase(int sampleCount) :
	m_pLeftChannel(NULL),
	m_pRightChannel(NULL),
	m_isActive(false),
	m_sampleCount(0)
{
	put_SampleCount(sampleCount);
}

SampleContainerBase::~SampleContainerBase()
{
	FreeChannels();
}

void SampleContainerBase::AllocChannels(int sampleCount)
{
	m_pLeftChannel = new SampleBuffer(sampleCount);
	if (NULL == m_pLeftChannel)
		throw new std::invalid_argument("SampleContainerBase: Not enough memory for SampleBuffer for left channel.");

	m_pRightChannel = new SampleBuffer(sampleCount);
	if (NULL == m_pRightChannel)
	{
		FreeChannels();
		throw new std::invalid_argument("SampleContainerBase: Not enough memory for SampleBuffer for right channel.");
	}
}

void SampleContainerBase::FreeChannels()
{
	if (NULL != m_pLeftChannel)
	{
		delete m_pLeftChannel;
		m_pLeftChannel = NULL;
	}
	if (NULL != m_pRightChannel)
	{
		delete m_pRightChannel;
		m_pRightChannel = NULL;
	}
	m_sampleCount = 0;
}

// virtual
ISampleBuffer* SampleContainerBase::get_LeftChannel()
{
	return m_pLeftChannel;
}

// virtual
ISampleBuffer* SampleContainerBase::get_RightChannel()
{
	return m_pRightChannel;
}

// virtual
bool SampleContainerBase::get_IsActive()
{
	return m_isActive;
}

// virtual
void SampleContainerBase::put_IsActive(bool value)
{
	m_isActive = value;
}

int SampleContainerBase::get_SampleCount()
{
	return m_sampleCount;
}

void SampleContainerBase::put_SampleCount(int sampleCount)
{
	if (sampleCount != m_sampleCount)
	{
		FreeChannels();
		AllocChannels(sampleCount);
		m_sampleCount = sampleCount;
	}
}