#include "pch.h"
#include "ContainerReader.h"


using namespace Audio::Foundation::Unmanaged;

ContainerReader::ContainerReader(ISampleContainerPtr& source) :
	m_pSource(source),
	m_sampleOffset(0),
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
		int sampleCount = std::min(m_pSource->SampleCount - m_sampleOffset, container->SampleCount);
		int channelCount = std::min(m_pSource->ChannelCount, container->ChannelCount);

		m_pSource->CopyTo(container, m_sampleOffset, sampleCount, 0, channelCount, 0, 0);
		m_sampleOffset += sampleCount;

		return sampleCount;
	}
	return 0;
}

