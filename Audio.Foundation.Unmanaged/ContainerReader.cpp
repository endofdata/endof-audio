#include "pch.h"
#include "ContainerReader.h"


using namespace Audio::Foundation::Unmanaged;

ContainerReader::ContainerReader(ISampleContainerPtr source, int sampleCount) :
	m_pSource(source),
	m_sampleCount(sampleCount),
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

ISampleProcessorPtr ContainerReader::get_First()
{
	return m_pFirst;
}

void ContainerReader::put_First(ISampleProcessorPtr value)
{
	m_pFirst = value;
}

bool ContainerReader::get_HasFirst()
{
	return m_pFirst != nullptr;
}

ISampleContainerPtr ContainerReader::get_Container()
{
	return m_span.Span(std::max(m_sampleOffset - m_sampleCount, 0), m_sampleCount, 0, m_pSource->ChannelCount);
}

void ContainerReader::OnNextBuffer(bool readSecondHalf)
{
	m_sampleOffset += m_sampleCount;

	if (HasFirst)
	{
		m_pFirst->Process(Container);
	}
}

