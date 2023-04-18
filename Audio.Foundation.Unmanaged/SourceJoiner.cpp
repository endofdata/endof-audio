#include "pch.h"
#include "SourceJoiner.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>
#include <functional>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SourceJoiner::SourceJoiner() :
	m_isBypassed(false),
	m_refCount(0)
{
}

SourceJoiner::~SourceJoiner()
{
}

IMPLEMENT_IUNKNOWN(SourceJoiner)

bool SourceJoiner::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISourceJoiner*>(this));
		return true;
	}
	if (iid == __uuidof(ISourceJoiner))
	{
		*ppvResult = dynamic_cast<ISourceJoiner*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	return false;
}

void SourceJoiner::AddSource(ISampleSourcePtr& Source)
{
	// do not allow two sends to the same destination
	RemoveSource(Source);

	m_vecSources.push_back(Source);
}

void SourceJoiner::RemoveSource(ISampleSourcePtr& Source)
{
	std::vector<ISampleSourcePtr>::iterator newEnd =
		remove_if(m_vecSources.begin(), m_vecSources.end(), [&Source](ISampleSourcePtr& item)
	{
		if (item == Source)
		{
			return true;
		}
		return false;
	});
	m_vecSources.erase(newEnd, m_vecSources.end());
}

void SourceJoiner::RemoveAllSources()
{
	m_vecSources.clear();
}

ISampleSourcePtr& SourceJoiner::get_Source(int index)
{
	return m_vecSources.at(index);
}

int SourceJoiner::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		int targetChannelCount = container->ChannelCount;

		if (targetChannelCount > 0)
		{
			std::for_each(m_vecSources.begin(), m_vecSources.end(), [this, &container](ISampleSourcePtr& item)
			{
				// Read in 'overub' mode to add sample values
				item->ReadSamples(container, true);
			});
		}
		return container->SampleCount;
	}
	return 0;
}

bool SourceJoiner::get_IsBypassed()
{
	return m_isBypassed;
}

void SourceJoiner::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}