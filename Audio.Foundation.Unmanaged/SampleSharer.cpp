#include "pch.h"
#include "SampleSharer.h"
#include <algorithm>
#include <functional>
#include <stdexcept>
#include "ObjectFactory.h"

using namespace std;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleSharer::SampleSharer() : 
	m_refCount(0)
{
}

SampleSharer::~SampleSharer()
{
}

IMPLEMENT_IUNKNOWN(SampleSharer)

bool SampleSharer::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleSharer*>(this));
		return true;
	}

	if (iid == __uuidof(ISampleSharer))
	{
		*ppvResult = dynamic_cast<ISampleSharer*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	return false;
}

void SampleSharer::AddTarget(ISampleProcessorPtr& target)
{
	target.AddRef();

	// do not allow two sends to the same destination
	RemoveTarget(target);

	m_vecTargets.push_back(target);
}

void SampleSharer::RemoveTarget(ISampleProcessorPtr& target)
{
	auto newEnd = remove_if(m_vecTargets.begin(), m_vecTargets.end(), [&target](ISampleProcessorPtr& item)
	{ 
		if(item == target)
		{
			return true;
		}
		return false;
	});
	m_vecTargets.erase(newEnd, m_vecTargets.end());
}

void SampleSharer::RemoveAllTargets()
{
	m_vecTargets.clear();
}

ISampleProcessorPtr& SampleSharer::get_Target(int index)
{
	return m_vecTargets.at(index);
}

void SampleSharer::Process(ISampleContainerPtr& container)
{
	std::for_each(m_vecTargets.begin(), m_vecTargets.end(), [&container](ISampleProcessorPtr& item)
	{
		item->Process(container);
	});
}