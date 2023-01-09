#include "pch.h"
#include "SampleContainerBase.h"
#include "SampleBuffer.h"
#include <stdexcept>
#include <algorithm>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainerBase::SampleContainerBase(int sampleCount, int channelCount) :
	m_isActive(false),
	m_sampleCount(0)
{
	CreateChannels(sampleCount, channelCount);
}

SampleContainerBase::~SampleContainerBase()
{
	FreeChannels();
}

void SampleContainerBase::CreateChannels(int sampleCount, int channelCount)
{
	if (sampleCount != get_SampleCount() || channelCount != get_ChannelCount())
	{
		if (sampleCount != get_SampleCount())
		{
			FreeChannels();
		}

		if (channelCount < get_ChannelCount())
		{
			for (int c = channelCount; c < get_ChannelCount(); c++)
			{
				ISampleBuffer* pBuffer = m_vecChannels.back();
				pBuffer->Release();
				m_vecChannels.pop_back();
			}
		}
		else
		{
			for (int c = get_ChannelCount(); c < channelCount; c++)
			{
				SampleBuffer* pBuffer = new SampleBuffer(sampleCount);
				pBuffer->AddRef();
				m_vecChannels.push_back(pBuffer);
			}
		}
		m_sampleCount = sampleCount;
	}
}

void SampleContainerBase::FreeChannels()
{
	for_each(m_vecChannels.begin(), m_vecChannels.end(), [](ISampleBuffer* item)
	{
		item->Release();
	});

	m_vecChannels.clear();
}

// virtual
int SampleContainerBase::get_SampleCount()
{
	return m_sampleCount;
}

// virtual
void SampleContainerBase::put_SampleCount(int sampleCount)
{
	CreateChannels(sampleCount, get_ChannelCount());
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

// virtual
int SampleContainerBase::get_ChannelCount()
{
	return m_vecChannels.size();
}

// virtual
void SampleContainerBase::put_ChannelCount(int channelCount)
{
	CreateChannels(m_sampleCount, channelCount);
}

// virtual
ISampleBuffer* SampleContainerBase::get_Channel(int index)
{
	if (index < 0 || index >= get_ChannelCount())
	{
		throw std::out_of_range("Invalid channel index");
	}
	return m_vecChannels.at(index);
}
