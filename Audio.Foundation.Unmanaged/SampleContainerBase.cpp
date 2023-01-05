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
	SampleBuffer* pBuffer = new SampleBuffer(sampleCount);
	pBuffer->AddRef();

	ISampleBuffer* pChannel = (ISampleBuffer*)InterlockedExchangePointer((void**)&m_pLeftChannel, pBuffer);

	if (NULL != pChannel)
	{
		pChannel->Release();
	}

	pBuffer = new SampleBuffer(sampleCount);
	pBuffer->AddRef();

	pChannel = (ISampleBuffer*)InterlockedExchangePointer((void**)&m_pRightChannel, new SampleBuffer(sampleCount));

	if (NULL != pChannel)
	{
		pChannel->Release();
	}
}

void SampleContainerBase::FreeChannels()
{
	ISampleBuffer* pChannel = (ISampleBuffer*)InterlockedExchangePointer((void**)&m_pLeftChannel, NULL);

	if (NULL != pChannel)
	{
		pChannel->Release();
	}

	pChannel = (ISampleBuffer*)InterlockedExchangePointer((void**)&m_pRightChannel, NULL);

	if (NULL != pChannel)
	{
		pChannel->Release();
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