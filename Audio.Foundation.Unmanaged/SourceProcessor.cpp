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

int SourceProcessor::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		if (context.IsSkipping)
		{
			m_pSource->SamplePosition = context.SamplePosition;
		}
		return m_pSource->ReadSamples(container, false);
	}
	return 0;
}

bool SourceProcessor::get_IsBypassed()
{
	return m_isBypassed;
}

void SourceProcessor::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}