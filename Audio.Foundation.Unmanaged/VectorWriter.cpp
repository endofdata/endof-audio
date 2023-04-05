#include "pch.h"
#include "VectorWriter.h"
#include "ObjectFactory.h"
#include <algorithm>
#include <functional>
#include "SampleContainer.h"
#include "SampleBuffer.h"

using namespace Audio::Foundation::Unmanaged;

VectorWriter::VectorWriter(int channelCount, int initialSize, int growth) :
	m_growth(growth),
	m_inUse(0),
	m_avail(0),
	m_isBypassed(false),
	m_refCount(0)
{
	if (initialSize == 0)
	{
		initialSize = 48000 * 60;
	}
	if (m_growth == 0)
	{
		m_growth = initialSize;
	}
	m_buffers.reserve(channelCount);

	for (int i = 0; i < channelCount; i++)
	{
		m_buffers.push_back(reinterpret_cast<Sample*>(std::malloc(sizeof(Sample) * initialSize)));
	}
	m_avail = initialSize;
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
	if (!m_isBypassed)
	{
		int samples = container->SampleCount;
		int maxSourceChannels = container->ChannelCount;
		int channel = 0;

		std::for_each(m_buffers.begin(), m_buffers.end(), [this, &channel, maxSourceChannels, &samples, container](Sample*& buffer)
		{
			const Sample* pSrc = container->Channels[channel]->SamplePtr;
			channel = (channel + 1) % maxSourceChannels;

			if (m_avail < m_inUse + samples)
			{
				int newSize = (div(m_inUse + samples, m_growth).quot + 1) * m_growth;
				Sample* newBuffer = reinterpret_cast<Sample*>(std::realloc(buffer, newSize * sizeof(Sample)));

				if (newBuffer == nullptr)
				{
					m_isBypassed = true;
					samples = m_avail - m_inUse;
				}
				else
				{
					buffer = newBuffer;
					m_avail = newSize;
				}
			}
			std::memcpy(&buffer[m_inUse], pSrc, samples * sizeof(Sample));
		});
		m_inUse += samples;
	}
}

ISampleContainerPtr VectorWriter::CreateSampleContainer()
{
	m_isBypassed = true;

	auto container = new SampleContainer(m_buffers, m_inUse);

	m_inUse = m_avail = 0;

	return container;
}

bool VectorWriter::get_IsBypassed()
{
	return m_isBypassed;
}

void VectorWriter::put_IsBypassed(bool value)
{
	m_isBypassed = value && (m_buffers.size() > 0);
}