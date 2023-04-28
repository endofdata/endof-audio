#include "pch.h"
#include "ContainerReader.h"


using namespace Audio::Foundation::Unmanaged;

ContainerReader::ContainerReader(ISampleContainerPtr& source) :
	m_container(source),
	m_samplePosition(0),
	m_isLooping(false),
	m_refCount(0),
	m_span(source)
{
	if (source == nullptr)
	{
		throw std::invalid_argument("'source' cannot be null.");
	}
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

int ContainerReader::ReadSamples(ISampleContainerPtr& container, const MixParameter& mix, bool overdub)
{
	if (m_samplePosition < m_container->SampleCount)
	{
		int channelCount = container->ChannelCount;
		int sampleCount = 0;
		int required = container->SampleCount;

		do
		{
			int sliceCount = std::min(m_container->SampleCount - m_samplePosition, required - sampleCount);
			m_container->WriteTo(container, m_samplePosition, sliceCount, 0, channelCount, 0, 0, mix, overdub);
			sampleCount += sliceCount;
			m_samplePosition += sliceCount;

			if (m_isLooping && m_samplePosition >= m_container->SampleCount)
			{
				m_samplePosition = 0;
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

int ContainerReader::get_SamplePosition() const
{
	return m_samplePosition;
}

void ContainerReader::put_SamplePosition(int value)
{
	m_samplePosition = value;
}

int ContainerReader::get_ChannelCount() const
{
	return m_container->ChannelCount;
}

void ContainerReader::put_ChannelCount(int value)
{
	m_container->ChannelCount = value;
}

