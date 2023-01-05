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

SampleSharer::SampleSharer() : m_refCount(0)
{
}

SampleSharer::~SampleSharer()
{
	RemoveAllSends();
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

void SampleSharer::AddSend(ISampleContainer& fromChannel, ISampleReceiver& toChannel, float volume, float pan)
{
	// do not allow two sends to the same destination
	RemoveSend(toChannel);

	IChannelLink* pChannelLink = ObjectFactory::CreateChannelLink(&fromChannel, &toChannel, volume, pan);
	if(NULL == pChannelLink)
	{
		RemoveAllSends();
		throw new std::invalid_argument("SampleSharer: Not enough memory for ChannelLink instance.");
	}

	m_vecSends.push_back(pChannelLink);
}

void SampleSharer::RemoveSend(ISampleReceiver& toChannel)
{
	vector<IChannelLink*>::iterator newEnd =
	remove_if(m_vecSends.begin(), m_vecSends.end(), [&toChannel](IChannelLink* pLink) 
	{ 
		if(pLink->Output == &toChannel)
		{
			pLink->Release();
			return true;
		}
		return false;
	});
	m_vecSends.erase(newEnd, m_vecSends.end());
}

void SampleSharer::RemoveAllSends()
{
	for_each(m_vecSends.begin(), m_vecSends.end(), [](IChannelLink* pLink)
	{
		pLink->Release();
	});
	m_vecSends.clear();
}

void SampleSharer::RouteToSends()
{
	for_each(m_vecSends.begin(), m_vecSends.end(), [](IChannelLink* pLink) 
	{ 
		pLink->Output->Receive(*pLink); 
	});
}

IChannelLink* SampleSharer::get_Send(int iIdx)
{
	IChannelLink* value = NULL;
	
	if (0 <= iIdx && iIdx < (int)m_vecSends.size())
	{
		value = m_vecSends.at(iIdx);
	}
	return value;
}
