#include "pch.h"
#include "SampleContainer.h"
#include "SampleBuffer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainer::SampleContainer(int sampleCount) :
	m_pLeftChannel(NULL),
	m_pRightChannel(NULL),
	m_isActive(false),
	m_sampleCount(0)
{
	SampleCount = sampleCount;
}

SampleContainer::~SampleContainer()
{
	FreeChannels();
}

bool SampleContainer::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(ISampleContainer))
	{
		*ppvResult = dynamic_cast<ISampleContainer*>(this);
		return true;
	}
	return UnknownBase::GetInterface(iid, ppvResult);
}


void SampleContainer::AllocChannels(int sampleCount)
{
	m_pLeftChannel = new SampleBuffer(sampleCount);
	if(NULL == m_pLeftChannel)
		throw new std::invalid_argument("SampleContainer: Not enough memory for SampleBuffer for left channel.");

	m_pRightChannel = new SampleBuffer(sampleCount);
	if(NULL == m_pRightChannel)
	{
		FreeChannels();
		throw new std::invalid_argument("SampleContainer: Not enough memory for SampleBuffer for right channel.");
	}
}

void SampleContainer::FreeChannels()
{
	if(NULL != m_pLeftChannel)
	{
		delete m_pLeftChannel;
		m_pLeftChannel = NULL;
	}
	if(NULL != m_pRightChannel)
	{
		delete m_pRightChannel;
		m_pRightChannel = NULL;
	}
	m_sampleCount = 0;
}

// virtual
ISampleBuffer* SampleContainer::get_LeftChannel()
{
	return m_pLeftChannel;
}

// virtual
ISampleBuffer* SampleContainer::get_RightChannel()
{
	return m_pRightChannel;
}

// virtual
bool SampleContainer::get_IsActive()
{
	return m_isActive;
}

// virtual
void SampleContainer::put_IsActive(bool value)
{
	m_isActive = value;
}

int SampleContainer::get_SampleCount()
{
	return m_sampleCount;
}

void SampleContainer::put_SampleCount(int sampleCount)
{
	if (sampleCount != m_sampleCount)
	{
		FreeChannels();
		AllocChannels(sampleCount);
		m_sampleCount = sampleCount;
	}
}