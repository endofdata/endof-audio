#include "pch.h"
#include "ContainerReader.h"


using namespace Audio::Foundation::Unmanaged;

ContainerReader::ContainerReader(const GUID& id, ISampleContainerPtr& source) :
	m_container(source),
	m_samplePosition(0),
	m_isLooping(false),
	m_refCount(0),
	m_span(source),
	m_id(id)
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

void* ContainerReader::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == __uuidof(ISampleSource))
	{
		return dynamic_cast<ISampleSource*>(this);
	}
	return nullptr;
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

		} while (m_isLooping && sampleCount < required);

		return sampleCount;
	}
	return 0;
}

const GUID& ContainerReader::get_Id() const
{
	return m_id;
}

bool ContainerReader::get_IsLooping() const
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

