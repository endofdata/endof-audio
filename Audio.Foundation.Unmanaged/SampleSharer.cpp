#include "pch.h"
#include "SampleSharer.h"
#include <algorithm>
#include <functional>
#include <stdexcept>
#include "ISampleReceiver.h"
#include "ObjectFactory.h"

using namespace std;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleSharer::SampleSharer() : 
	m_refCount(0), 
	m_pSource(nullptr)
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
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}

	if (iid == __uuidof(ISampleSharer))
	{
		*ppvResult = dynamic_cast<ISampleSharer*>(this);
		return true;
	}
	return false;
}

void SampleSharer::AddTarget(ISampleProcessorPtr target)
{
	target.AddRef();

	// do not allow two sends to the same destination
	RemoveTarget(target);

	m_vecTargets.push_back(target);
}

void SampleSharer::RemoveTarget(ISampleProcessorPtr target)
{
	vector<ISampleProcessorPtr>::iterator newEnd =
	remove_if(m_vecTargets.begin(), m_vecTargets.end(), [&target](ISampleReceiverPtr item) 
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

void SampleSharer::Push()
{
	if (m_pSource != nullptr)
	{
		for_each(m_vecTargets.begin(), m_vecTargets.end(), [this](ISampleProcessorPtr item)
		{
			item->Process(m_pSource);
		});
	}
}

void SampleSharer::put_Source(ISampleContainerPtr value)
{
	m_pSource = value;
}

ISampleContainerPtr SampleSharer::get_Source()
{
	return m_pSource;
}

ISampleProcessorPtr SampleSharer::get_Target(int index)
{
	return m_vecTargets.at(index);
}
