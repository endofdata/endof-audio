#include "pch.h"
#include "ContainerWriter.h"

using namespace Audio::Foundation::Unmanaged;


ContainerWriter::ContainerWriter(ISampleContainerPtr& target) :
	m_pTarget(target),
	m_isBypassed(false),
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

int ContainerWriter::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		return container->WriteTo(m_pTarget, 0, container->SampleCount, 0, container->ChannelCount, 0, 0, MixParameter(), false);
	}
	return 0;
}

bool ContainerWriter::get_IsBypassed() const
{
	return m_isBypassed;
}

void ContainerWriter::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}