#include "pch.h"
#include "SampleContainerSpan.h"
#include "SampleBuffer.h"
#include "ObjectFactory.h"
#include "SampleContainerSpan.h"
#include "SampleBufferSpan.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainerSpan::SampleContainerSpan() :
	m_sampleCount(0),
	m_refCount(0)
{
}

SampleContainerSpan::SampleContainerSpan(std::vector<ISampleBufferPtr> vecChannels, int sampleCount) :
	m_sampleCount(sampleCount),
	m_refCount(0)
{
	m_vecChannels = vecChannels;
	if (sampleCount == 0 && m_vecChannels.size() > 0)
	{
		m_sampleCount = m_vecChannels[0]->SampleCount;
	}
}

SampleContainerSpan::SampleContainerSpan(ISampleContainerPtr source) :
	m_source(source),
	m_sampleCount(0),
	m_refCount(0)
{
	if (source != nullptr)
	{
		int channelCount = source->ChannelCount;

		for (int c = 0; c < channelCount; c++)
		{
			m_vecChannels.push_back(m_source->Channels[c]);
		}
		m_sampleCount = m_source->SampleCount;
	}
}

SampleContainerSpan::~SampleContainerSpan()
{
}

IMPLEMENT_IUNKNOWN(SampleContainerSpan)

bool SampleContainerSpan::GetInterface(REFIID iid, void** ppvResult)
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

int SampleContainerSpan::get_SampleCount() const
{
	return m_sampleCount;
}

void SampleContainerSpan::put_SampleCount(int sampleCount)
{
	throw std::runtime_error("SampleContainerSpan cannot change sample count");
}

int SampleContainerSpan::get_ChannelCount() const
{
	return (int)m_vecChannels.size();
}

void SampleContainerSpan::put_ChannelCount(int channelCount)
{
	throw std::runtime_error("SampleContainerSpan cannot change channel count");
}

ISampleBufferPtr SampleContainerSpan::get_Channel(int index)
{
	if (index < 0 || index >= get_ChannelCount())
	{
		throw std::out_of_range("Invalid channel index");
	}
	return m_vecChannels.at(index);
}

int SampleContainerSpan::AddTo(ISampleContainerPtr other, int sampleOffset, int sampleCount, int channelOffset, int channelCount, int targetSampleOffset, int targetChannelOffset) const
{
	int maxChannels = std::min(channelCount, std::min(other->ChannelCount - targetChannelOffset, ChannelCount - channelOffset));
	int done = 0;

	for (int c = 0; c < maxChannels; c++)
	{
		done = m_vecChannels[c + channelOffset]->AddTo(other->Channels[c + targetChannelOffset], sampleOffset, sampleCount, targetSampleOffset);
	}
	return done;
}

int SampleContainerSpan::CopyTo(ISampleContainerPtr other, int sampleOffset, int sampleCount, int channelOffset, int channelCount, int targetSampleOffset, int targetChannelOffset) const
{
	int maxChannels = std::min(channelCount, std::min(other->ChannelCount - targetChannelOffset, ChannelCount - channelOffset));
	int done = 0;

	for (int c = 0; c < maxChannels; c++)
	{
		done = m_vecChannels[c + channelOffset]->CopyTo(other->Channels[c + targetChannelOffset], sampleOffset, sampleCount, targetSampleOffset);
	}
	return done;
}

ISampleContainerPtr SampleContainerSpan::Span(int sampleOffset, int sampleCount, int channelOffset, int channelCount)
{
	int maxChannels = std::min(channelCount, ChannelCount - channelOffset);
	int offset = std::min(SampleCount, sampleOffset);
	int count = std::min(SampleCount - offset, sampleCount);

	auto buffers = std::vector<ISampleBufferPtr>(maxChannels);
	buffers.reserve(maxChannels);

	for (int c = 0; c < maxChannels; c++)
	{
		buffers[c] = new SampleBufferSpan(&Channels[channelOffset + c]->SamplePtr[offset], count);
	}

	return new SampleContainerSpan(buffers);
}

