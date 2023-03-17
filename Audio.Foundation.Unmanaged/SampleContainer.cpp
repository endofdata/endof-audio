#include "pch.h"
#include "SampleContainer.h"
#include "ObjectFactory.h"
#include "SampleContainerSpan.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainer::SampleContainer(int sampleCount, int channelCount)
{
	CreateChannels(sampleCount, channelCount);
}

SampleContainer::~SampleContainer()
{
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
