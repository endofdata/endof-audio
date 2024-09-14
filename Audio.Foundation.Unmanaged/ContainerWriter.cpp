#include "pch.h"
#include "ContainerWriter.h"

using namespace Audio::Foundation::Unmanaged;


ContainerWriter::ContainerWriter(ISampleContainerPtr& target, const MixParameter& mix) :
	m_pTarget(target),
	m_isBypassed(false),
	m_mix(mix),
	m_refCount(0)
{
}

ContainerWriter::~ContainerWriter()
{
}

IMPLEMENT_IUNKNOWN(ContainerWriter)

void* ContainerWriter::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		return dynamic_cast<ISampleProcessor*>(this);
	}
	return nullptr;
}

int ContainerWriter::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		return container->WriteTo(m_pTarget, 0, container->SampleCount, 0, container->ChannelCount, 0, 0, m_mix, false);
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