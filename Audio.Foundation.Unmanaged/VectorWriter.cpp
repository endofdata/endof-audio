#include "pch.h"
#include "VectorWriter.h"
#include "ObjectFactory.h"
#include <algorithm>
#include <functional>

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

ISampleProcessorPtr VectorWriter::get_Next()
{
	return m_pNext;
}

void VectorWriter::put_Next(ISampleProcessorPtr value)
{
	m_pNext = value;
}

bool VectorWriter::get_HasNext()
{
	return m_pNext != nullptr;
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

	if (HasNext)
	{
		m_pNext->Process(container);
	}
}

ISampleContainerPtr VectorWriter::CreateSampleContainer()
{
	return ObjectFactory::CreateSampleContainer(m_inUse, m_buffers.size());
}
