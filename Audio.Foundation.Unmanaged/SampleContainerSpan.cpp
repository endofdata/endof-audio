#include "pch.h"
#include "SampleContainerSpan.h"
#include "SampleBuffer.h"
#include "ObjectFactory.h"
#include "SampleContainerSpan.h"
#include "SampleBufferSpan.h"
#include "SampleConversionUnmanaged.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainerSpan::SampleContainerSpan() :
	m_sampleCount(0),
	m_refCount(0)
{
}

SampleContainerSpan::SampleContainerSpan(const std::vector<ISampleBufferPtr>& vecChannels, int sampleCount) :
	m_sampleCount(sampleCount),
	m_refCount(0)
{
	m_vecChannels = vecChannels;
	if (sampleCount == 0 && m_vecChannels.size() > 0)
	{
		m_sampleCount = m_vecChannels[0]->SampleCount;
	}
}

SampleContainerSpan::SampleContainerSpan(ISampleContainerPtr& source) :
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

void* SampleContainerSpan::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == __uuidof(ISampleContainer))
	{
		return dynamic_cast<ISampleContainer*>(this);
	}
	return nullptr;
}

int SampleContainerSpan::get_SampleCount() const
{
	return m_sampleCount;
}

void SampleContainerSpan::put_SampleCount(int sampleCount)
{
	CreateChannels(sampleCount, get_ChannelCount());
}

int SampleContainerSpan::get_ChannelCount() const
{
	return (int)m_vecChannels.size();
}

void SampleContainerSpan::put_ChannelCount(int channelCount)
{
	CreateChannels(get_SampleCount(), channelCount);
}

ISampleBufferPtr SampleContainerSpan::get_Channel(int index)
{
	if (index < 0 || index >= get_ChannelCount())
	{
		throw std::out_of_range("Invalid channel index");
	}
	return m_vecChannels.at(index);
}

void SampleContainerSpan::Clear()
{
	for (int c = 0; c < ChannelCount; c++)
	{
		Channels[c]->Clear();
	}
}

int SampleContainerSpan::WriteTo(ISampleContainerPtr& other, int sampleOffset, int sampleCount, int channelOffset, int channelCount, 
	int targetSampleOffset, int targetChannelOffset, const MixParameter& mix, bool overdub) const
{
	int sourceChannels = std::min(ChannelCount - channelOffset, channelCount);
	int done = 0;
	
	for (int c = 0; c < channelCount; c++)
	{
		ISampleBufferPtr pChannel = other->Channels[c + targetChannelOffset];
		double channelLevel = (c & 1) ? mix.FactorRight : mix.FactorLeft;
		done = m_vecChannels[(c % sourceChannels) + channelOffset]->WriteTo(pChannel, sampleOffset, sampleCount, targetSampleOffset, channelLevel, overdub);
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

void SampleContainerSpan::CreateChannels(int sampleCount, int channelCount)
{
	throw std::runtime_error("Change of sample count or channel count is not supported.");
}