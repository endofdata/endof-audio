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

bool TransportEvents::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}

	if (iid == __uuidof(ITransportEvents))
	{
		*ppvResult = dynamic_cast<ITransportEvents*>(this);
		return true;
	}

	*ppvResult = nullptr;
	return false;
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
