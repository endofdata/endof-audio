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

bool SourceJoiner::RemoveSource(ISampleSourcePtr& source)
{
	bool isRemoved = false;

	auto newEnd =
		remove_if(m_vecSources.begin(), m_vecSources.end(), [source, &isRemoved](std::pair<ISampleSourcePtr, MixParameter>& item)
	{
		if (item.first == source)
		{
			isRemoved = true;
			return true;
		}
		return false;
	});
	m_vecSources.erase(newEnd, m_vecSources.end());

	return isRemoved;
}

bool SourceJoiner::RemoveSource(const GUID& id)
{
	bool isRemoved = false;

	auto newEnd =
		remove_if(m_vecSources.begin(), m_vecSources.end(), [id, &isRemoved](std::pair<ISampleSourcePtr, MixParameter>& item)
	{
		if (item.first->Id == id)
		{
			isRemoved = true;
			return true;
		}
		return false;
	});
	m_vecSources.erase(newEnd, m_vecSources.end());

	return isRemoved;
}

void SourceJoiner::RemoveAllSources()
{
	m_vecSources.clear();
}

int SourceJoiner::get_SourceCount() const
{
	return static_cast<int>(m_vecSources.size());
}

ISampleSourcePtr& SourceJoiner::get_Source(const GUID& id)
{
	auto found = std::find_if(m_vecSources.begin(), m_vecSources.end(), [id](std::pair<ISampleSourcePtr, MixParameter>& item)
	{
		return item.first->Id == id;
	});

	return found->first;
}

MixParameter& SourceJoiner::get_Parameter(const GUID& id)
{
	auto found = std::find_if(m_vecSources.begin(), m_vecSources.end(), [id](std::pair<ISampleSourcePtr, MixParameter>& item)
	{
		return item.first->Id == id;
	});

	return found->second;
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