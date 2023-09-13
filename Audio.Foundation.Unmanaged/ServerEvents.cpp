#include "pch.h"
#include "ServerEvents.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;
using namespace Audio::Foundation::Unmanaged;

ServerEvents::ServerEvents() :
	m_context(nullptr),
	m_onRequestData(nullptr),
	m_onContinueResponse(nullptr),
	m_refCount(0)
{
}

IMPLEMENT_IUNKNOWN(ServerEvents)

void* ServerEvents::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == __uuidof(IServerEvents))
	{
		return dynamic_cast<IServerEvents*>(this);
	}
	return nullptr;
}


void* ServerEvents::get_Context() const
{
	return m_context;
}

void ServerEvents::put_Context(void* value)
{
	m_context = value;
}

RequestDataHandler ServerEvents::get_OnRequestData() const
{
	return m_onRequestData;
}

void ServerEvents::put_OnRequestData(RequestDataHandler value)
{
	m_onRequestData = value;
}


ContinueResponseHandler ServerEvents::get_OnContinueResponse() const
{
	return m_onContinueResponse;
}

void ServerEvents::put_OnContinueResponse(ContinueResponseHandler value)
{
	m_onContinueResponse = value;
}

