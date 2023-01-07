#include "pch.h"
#include "SampleSharer.h"
#include "ChannelLink.h"
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
	m_pSource(NULL)
{
}

SampleSharer::~SampleSharer()
{
	RemoveAllTargets();
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

void SampleSharer::AddTarget(ISampleReceiver& target)
{
	target.AddRef();

	// do not allow two sends to the same destination
	RemoveTarget(target);

	m_vecTargets.push_back(&target);
}

void SampleSharer::RemoveTarget(ISampleReceiver& target)
{
	vector<ISampleReceiver*>::iterator newEnd =
	remove_if(m_vecTargets.begin(), m_vecTargets.end(), [&target](ISampleReceiver* item) 
	{ 
		if(item == &target)
		{
			item->Release();
			return true;
		}
		return false;
	});
	m_vecTargets.erase(newEnd, m_vecTargets.end());
}

void SampleSharer::RemoveAllTargets()
{
	for_each(m_vecTargets.begin(), m_vecTargets.end(), [](ISampleReceiver* item)
	{
		item->Release();
	});
	m_vecTargets.clear();
}

void SampleSharer::RouteToTargets()
{
	if (m_pSource != NULL)
	{
		for_each(m_vecTargets.begin(), m_vecTargets.end(), [this](ISampleReceiver* item)
		{
			item->Receive(*m_pSource);
		});
	}
}

void SampleSharer::put_Source(ISampleContainer* value)
{
	if (value != NULL)
	{
		value->AddRef();
	}

	ISampleContainer* pSource = (ISampleContainer*)InterlockedExchangePointer((void**)&m_pSource, value);

	if (pSource != NULL)
	{
		pSource->Release();
	}
}

ISampleContainer* SampleSharer::get_Source()
{
	return m_pSource;
}

ISampleReceiver* SampleSharer::get_Target(int index)
{
	ISampleReceiver* value = NULL;
	
	if (0 <= index && index < (int)m_vecTargets.size())
	{
		value = m_vecTargets.at(index);
	}
	return value;
}
