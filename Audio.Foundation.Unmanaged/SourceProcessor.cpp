#include "pch.h"
#include "SourceProcessor.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>
#include <functional>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SourceProcessor::SourceProcessor(ISampleSourcePtr& source) :
	m_pSource(source),
	m_refCount(0)
{
}

SourceProcessor::~SourceProcessor()
{
}

IMPLEMENT_IUNKNOWN(SourceProcessor)

bool SourceProcessor::GetInterface(REFIID iid, void** ppvResult)
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

void SourceProcessor::Process(ISampleContainerPtr& container)
{
	m_pSource->ReadSamples(container);
}