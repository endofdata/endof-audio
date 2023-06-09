#include "pch.h"
#include "SampleSharer.h"
#include <algorithm>
#include <functional>
#include <stdexcept>
#include "FoundationObjectFactory.h"

using namespace std;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleSharer::SampleSharer() : 
	m_isBypassed(false),
	m_refCount(0)
{
}

SampleSharer::~SampleSharer()
{
}

IMPLEMENT_IUNKNOWN(SampleSharer)

void* SampleSharer::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(dynamic_cast<ISampleSharer*>(this));
	}

	if (iid == __uuidof(ISampleSharer))
	{
		return dynamic_cast<ISampleSharer*>(this);
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		return dynamic_cast<ISampleProcessor*>(this);
	}
	return nullptr;
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

int SampleSharer::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		std::for_each(m_vecTargets.begin(), m_vecTargets.end(), [&container, context](ISampleProcessorPtr& item)
		{
			item->Process(container, context);
		});
		return container->SampleCount;
	}
	return 0;
}

bool SampleSharer::get_IsBypassed() const
{
	return m_isBypassed;
}

void SampleSharer::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}