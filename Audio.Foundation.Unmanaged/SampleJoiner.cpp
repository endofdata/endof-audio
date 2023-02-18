#include "pch.h"
#include "SampleJoiner.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>
#include <functional>
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleJoiner::SampleJoiner() :
	m_pTarget(nullptr),
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
	return false;
}

void SampleJoiner::AddSource(ISampleContainerPtr Source)
{
	Source.AddRef();

	// do not allow two sends to the same destination
	RemoveSource(Source);

	m_vecSources.push_back(Source);
}

void SampleJoiner::RemoveSource(ISampleContainerPtr Source)
{
	std::vector<ISampleContainerPtr>::iterator newEnd =
		remove_if(m_vecSources.begin(), m_vecSources.end(), [&Source](ISampleContainerPtr item)
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

ISampleProcessorPtr SampleJoiner::get_Target()
{
	return m_pTarget;
}

void SampleJoiner::put_Target(ISampleProcessorPtr value)
{
	m_pTarget = value;
}

ISampleContainerPtr SampleJoiner::get_Source(int index)
{
	return m_vecSources.at(index);
}

