#include "pch.h"
#include "SampleSharer.h"
#include "ChannelLink.h"
#include <algorithm>
#include <functional>
#include <stdexcept>
#include "ISampleReceiver.h"

using namespace std;
using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleSharer::SampleSharer()
{
}

SampleSharer::~SampleSharer()
{
	RemoveAllSends();
}

bool SampleSharer::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(ISampleSharer))
	{
		*ppvResult = dynamic_cast<ISampleSharer*>(this);
		return true;
	}
	return UnknownBase::GetInterface(iid, ppvResult);
}

void SampleSharer::AddSend(ISampleContainer& fromChannel, ISampleReceiver& toChannel, float volume, float pan)
{
	// do not allow two sends to the same destination
	RemoveSend(toChannel);

	ChannelLink* pChannelLink = new ChannelLink(&fromChannel, &toChannel, volume, pan);
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
			delete pLink;
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
		delete pLink;
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
	
	if(0 <= iIdx && iIdx < (int)m_vecSends.size()) 
		value = m_vecSends.at(iIdx);

	return value;
}
