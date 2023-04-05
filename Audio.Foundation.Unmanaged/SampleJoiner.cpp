#include "pch.h"
#include "SampleJoiner.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>
#include <functional>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleJoiner::SampleJoiner() :
	m_isBypassed(false),
	m_refCount(0)
{
}

SampleJoiner::~SampleJoiner()
{
}

IMPLEMENT_IUNKNOWN(SampleJoiner)

bool SampleJoiner::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleJoiner*>(this));
		return true;
	}
	if (iid == __uuidof(ISampleJoiner))
	{
		*ppvResult = dynamic_cast<ISampleJoiner*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	return false;
}

void SampleJoiner::AddSource(ISampleContainerPtr& Source)
{
	Source.AddRef();

	// do not allow two sends to the same destination
	RemoveSource(Source);

	m_vecSources.push_back(Source);
}

void SampleJoiner::RemoveSource(ISampleContainerPtr& Source)
{
	std::vector<ISampleContainerPtr>::iterator newEnd =
		remove_if(m_vecSources.begin(), m_vecSources.end(), [&Source](ISampleContainerPtr& item)
	{
		if (item == Source)
		{
			return true;
		}
		return false;
	});
	m_vecSources.erase(newEnd, m_vecSources.end());
}

void SampleJoiner::RemoveAllSources()
{
	m_vecSources.clear();
}

ISampleContainerPtr& SampleJoiner::get_Source(int index)
{
	return m_vecSources.at(index);
}

void SampleJoiner::Process(ISampleContainerPtr& container)
{
	if (!m_isBypassed)
	{
		int targetChannelCount = container->ChannelCount;

		if (targetChannelCount > 0)
		{
			std::for_each(m_vecSources.begin(), m_vecSources.end(), [this, &container](ISampleContainerPtr& item)
			{
				item->AddTo(container, 0, item->SampleCount, 0, item->ChannelCount, 0, 0);
			});
		}
	}
}

bool SampleJoiner::get_IsBypassed()
{
	return m_isBypassed;
}

void SampleJoiner::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}