#include "pch.h"
#include "VectorWriter.h"
#include "ObjectFactory.h"
#include <algorithm>
#include <functional>
#include "SampleContainerSpan.h"
#include "SampleBufferSpan.h"

using namespace Audio::Foundation::Unmanaged;

VectorWriter::VectorWriter(int channelCount, int initialSize, int growth) :
	m_growth(growth),
	m_inUse(0),
	m_refCount(0)
{
	m_buffers.reserve(channelCount);

	for (int i = 0; i < channelCount; i++)
	{
		m_buffers.push_back(std::vector<Sample>(initialSize, 0.0));
	}
}

VectorWriter::~VectorWriter()
{
}

IMPLEMENT_IUNKNOWN(VectorWriter)

bool VectorWriter::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleContainer))
	{
		*ppvResult = CreateSampleContainer().Detach();
		return true;
	}
	return false;
}

void VectorWriter::Process(ISampleContainerPtr& container)
{
	int samples = container->SampleCount;
	int maxSourceChannels = container->ChannelCount;
	int channel = 0;

	std::for_each(m_buffers.begin(), m_buffers.end(), [this, &channel, maxSourceChannels, samples, container](std::vector<Sample>& buffer)
	{
		const Sample* pSrc = container->Channels[channel]->SamplePtr;
		channel = (channel + 1) % maxSourceChannels;

		if (buffer.size() < m_inUse + samples)
		{
			buffer.reserve((div(m_inUse + samples, m_growth).quot + 1) * m_growth);
		}
		std::memcpy(&buffer.data()[m_inUse], pSrc, samples * sizeof(Sample));
	});
	m_inUse += samples;
}

ISampleContainerPtr VectorWriter::CreateSampleContainer()
{
	int channel = 0;
	std::vector<ISampleBufferPtr> bufferPointers;

	std::for_each(m_buffers.begin(), m_buffers.end(), [this, &bufferPointers](std::vector<Sample>& buffer)
	{
		ISampleBufferPtr sampleBuffer = new SampleBufferSpan(buffer.data(), m_inUse);

		bufferPointers.push_back(sampleBuffer);
	});
	return new SampleContainerSpan(bufferPointers, m_inUse);
}
