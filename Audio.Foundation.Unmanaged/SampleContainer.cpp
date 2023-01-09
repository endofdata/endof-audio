#include "pch.h"
#include "SampleContainer.h"
#include "SampleBuffer.h"
#include <stdexcept>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleContainer::SampleContainer(int sampleCount, int channelCount) : 
	SampleContainerBase(sampleCount, channelCount),
	m_refCount(0)
{
}

SampleContainer::~SampleContainer()
{
}

IMPLEMENT_IUNKNOWN(SampleContainer)

IMPLEMENT_SAMPLECONTAINER(SampleContainer)

bool SampleContainer::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleContainer))
	{
		*ppvResult = dynamic_cast<ISampleContainer*>(this);
		return true;
	}
	return false;
}

