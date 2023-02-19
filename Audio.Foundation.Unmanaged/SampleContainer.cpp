#include "pch.h"
#include "SampleContainer.h"
#include "SampleBuffer.h"
#include "ObjectFactory.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainer::SampleContainer(int sampleCount, int channelCount) : 
	m_isActive(false),
	m_sampleCount(0),
	m_refCount(0)
{
	CreateChannels(sampleCount, channelCount);
}

SampleContainer::~SampleContainer()
{
}

IMPLEMENT_IUNKNOWN(SampleContainer)

bool SampleContainer::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleContainer))
	{
		*ppvResult = dynamic_cast<ISampleContainer*>(this);
		return true;
	}
	return false;
}

void SampleContainer::CreateChannels(int sampleCount, int channelCount)
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
				m_vecChannels.push_back(ObjectFactory::CreateSampleBuffer(sampleCount));
			}
		}
		m_sampleCount = sampleCount;
	}
}

int SampleContainer::get_SampleCount()
{
	return m_sampleCount;
}

void SampleContainer::put_SampleCount(int sampleCount)
{
	CreateChannels(sampleCount, get_ChannelCount());
}

int SampleContainer::get_ChannelCount()
{
	return (int)m_vecChannels.size();
}

void SampleContainer::put_ChannelCount(int channelCount)
{
	CreateChannels(m_sampleCount, channelCount);
}

ISampleBufferPtr SampleContainer::get_Channel(int index)
{
	if (index < 0 || index >= get_ChannelCount())
	{
		throw std::out_of_range("Invalid channel index");
	}
	return m_vecChannels.at(index);
}

