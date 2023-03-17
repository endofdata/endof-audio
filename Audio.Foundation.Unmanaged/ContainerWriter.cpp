#include "pch.h"
#include "ContainerWriter.h"

using namespace Audio::Foundation::Unmanaged;


ContainerWriter::ContainerWriter(ISampleContainerPtr& target) :
	m_pTarget(target),
	m_pNext(nullptr),
	m_refCount(0)
{
}

ContainerWriter::~ContainerWriter()
{
}

IMPLEMENT_IUNKNOWN(ContainerWriter)

bool ContainerWriter::GetInterface(REFIID iid, void** ppvResult)
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
	return false;
}

ISampleProcessorPtr ContainerWriter::get_Next()
{
	return m_pNext;
}

void ContainerWriter::put_Next(ISampleProcessorPtr value)
{
	m_pNext = value;
}


bool ContainerWriter::get_HasNext()
{
	return m_pNext != nullptr;
}


void ContainerWriter::Process(ISampleContainerPtr& container)
{
	container->CopyTo(m_pTarget, 0, container->SampleCount, 0, container->ChannelCount, 0, 0);

	if (HasNext)
	{
		m_pNext->Process(container);
	}
}
