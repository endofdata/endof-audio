#include "pch.h"
#include "SampleContainer.h"
#include "FoundationObjectFactory.h"
#include "SampleContainerSpan.h"
#include "SampleBuffer.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainer::SampleContainer(int sampleCount, int channelCount)
{
	CreateChannels(sampleCount, channelCount);
}

SampleContainer::SampleContainer(std::vector<Sample*>& buffers, int sampleCount)
{
	std::for_each(buffers.begin(), buffers.end(), [this, sampleCount](Sample* buffer)
	{
		ISampleBufferPtr sampleBuffer = new SampleBuffer(buffer, sampleCount);
		m_vecChannels.push_back(sampleBuffer);
	});
	m_sampleCount = sampleCount;
	buffers.clear();
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
				m_vecChannels.push_back(FoundationObjectFactory::CreateSampleBuffer(sampleCount));
			}
		}
		m_sampleCount = sampleCount;
	}
}
