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
}

void SampleContainerBase::CreateChannels(int sampleCount, int channelCount)
{
	int currentSamples = get_SampleCount();
	int currentChannels = get_ChannelCount();

	if (sampleCount != currentSamples || channelCount != currentChannels)
	{
		if (sampleCount != currentSamples)
		{
			m_vecChannels.clear();
			currentChannels = 0;
		}

		if (channelCount < currentChannels)
		{
			for (int c = channelCount; c < currentChannels; c++)
			{
				m_vecChannels.pop_back();
			}
		}
		else
		{
			for (int c = currentChannels; c < channelCount; c++)
			{
				m_vecChannels.push_back(new SampleBuffer(sampleCount));
			}
		}
		m_sampleCount = sampleCount;
	}
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
	return (int)m_vecChannels.size();
}

// virtual
void SampleContainerBase::put_ChannelCount(int channelCount)
{
	CreateChannels(m_sampleCount, channelCount);
}

// virtual
ISampleBufferPtr SampleContainerBase::get_Channel(int index)
{
	if (index < 0 || index >= get_ChannelCount())
	{
		throw std::out_of_range("Invalid channel index");
	}
	return m_vecChannels.at(index);
}
