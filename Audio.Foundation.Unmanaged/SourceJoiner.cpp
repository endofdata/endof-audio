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

void* SourceJoiner::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(dynamic_cast<ISourceJoiner*>(this));
	}
	if (iid == __uuidof(ISourceJoiner))
	{
		return dynamic_cast<ISourceJoiner*>(this);
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		return dynamic_cast<ISampleProcessor*>(this);
	}
	return nullptr;
}

void SourceJoiner::AddSource(ISampleSourcePtr& source, const MixParameter& mix)
{
	// do not allow two sends to the same destination
	RemoveSource(source);

	m_vecSources.push_back(std::pair<ISampleSourcePtr, MixParameter>(source, mix));
}

void SourceJoiner::RemoveSource(ISampleSourcePtr& source)
{
	auto newEnd =
		remove_if(m_vecSources.begin(), m_vecSources.end(), [&source](std::pair<ISampleSourcePtr, MixParameter>& item)
	{
		if (item.first == source)
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
	return m_vecSources.at(index).first;
}

MixParameter& SourceJoiner::get_MixParameter(int index)
{
	return m_vecSources.at(index).second;
}

int SourceJoiner::get_SourceCount() const
{
	return static_cast<int>(m_vecSources.size());
}

int SourceJoiner::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		int targetChannelCount = container->ChannelCount;

		if (targetChannelCount > 0)
		{
			std::for_each(m_vecSources.begin(), m_vecSources.end(), [this, &container](std::pair<ISampleSourcePtr, MixParameter>& item)
			{
				// Read in 'overdub' mode to add sample values
				item.first->ReadSamples(container, item.second, true);
			});
		}
		return container->SampleCount;
	}
	return 0;
}

bool SourceJoiner::get_IsBypassed() const
{
	return m_isBypassed;
}

void SourceJoiner::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}