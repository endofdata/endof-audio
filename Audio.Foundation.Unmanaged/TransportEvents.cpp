#include "pch.h"
#include "TransportEvents.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

TransportEvents::TransportEvents() :
	m_pContext(nullptr),
	m_onTransport(nullptr),
	m_refCount(0)
{
}

TransportEvents::~TransportEvents()
{
}

IMPLEMENT_IUNKNOWN(TransportEvents)

void* TransportEvents::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}

	if (iid == __uuidof(ITransportEvents))
	{
		return dynamic_cast<ITransportEvents*>(this);
	}
	return nullptr;
}

void* TransportEvents::get_Context()
{
	return m_pContext;
}

void TransportEvents::put_Context(void* value)
{
	m_pContext = value;
}

TransportEventHandler TransportEvents::get_OnTransport()
{
	return m_onTransport;
}
void TransportEvents::put_OnTransport(TransportEventHandler value)
{
	m_onTransport = value;
}

void TransportEvents::Transport(TransportCode code)
{
	Invoke(m_onTransport, code);
}

void TransportEvents::Invoke(TransportEventHandler handler, TransportCode code)
{
	void* pContext = m_pContext;

	if (pContext != nullptr && handler != nullptr)
	{
		handler(pContext, code);
	}
}
