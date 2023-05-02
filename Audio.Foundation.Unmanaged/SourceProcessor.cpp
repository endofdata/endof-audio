#include "pch.h"
#include "SourceProcessor.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>
#include <functional>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SourceProcessor::SourceProcessor(ISampleSourcePtr& source) :
	m_pSource(source),
	m_isBypassed(false),
	m_refCount(0)
{
}

SourceProcessor::~SourceProcessor()
{
}

IMPLEMENT_IUNKNOWN(SourceProcessor)

void* SourceProcessor::GetInterface(REFIID iid)
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

int SourceProcessor::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		if (context.IsLoopStart)
		{
			m_pSource->SamplePosition = context.SamplePosition;
		}
		return m_pSource->ReadSamples(container, MixParameter(), false);
	}
	return 0;
}

bool SourceProcessor::get_IsBypassed() const
{
	return m_isBypassed;
}

void SourceProcessor::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}