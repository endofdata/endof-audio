#include "pch.h"
#include "ContainerReader.h"


using namespace Audio::Foundation::Unmanaged;

ContainerReader::ContainerReader(ISampleContainerPtr& source) :
	m_pSource(source),
	m_sampleOffset(0),
	m_isLooping(false),
	m_refCount(0),
	m_span(source)
{
}

ContainerReader::~ContainerReader()
{
}

IMPLEMENT_IUNKNOWN(ContainerReader)

bool ContainerReader::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleSource))
	{
		*ppvResult = dynamic_cast<ISampleSource*>(this);
		return true;
	}
	return false;
}

int ContainerReader::ReadSamples(ISampleContainerPtr& container)
{
	if (m_sampleOffset < m_pSource->SampleCount)
	{
		int channelCount = std::min(m_pSource->ChannelCount, container->ChannelCount);
		int sampleCount = 0;
		int required = container->SampleCount;

		do
		{
			int sliceCount = std::min(m_pSource->SampleCount - m_sampleOffset, required - sampleCount);
			m_pSource->CopyTo(container, m_sampleOffset, sliceCount, 0, channelCount, 0, 0);
			sampleCount += sliceCount;
			m_sampleOffset += sliceCount;

			if (m_isLooping && m_sampleOffset >= m_pSource->SampleCount)
			{
				m_sampleOffset = 0;
			}

		} while (m_isLooping && sampleCount < container->SampleCount);

		return sampleCount;
	}
	return 0;
}

bool ContainerReader::get_IsLooping()
{
	return m_isLooping;
}

void ContainerReader::put_IsLooping(bool value)
{
	m_isLooping = value;
}
