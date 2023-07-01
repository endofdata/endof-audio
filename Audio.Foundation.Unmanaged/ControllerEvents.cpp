#include "pch.h"
#include "ControllerEvents.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

ControllerEvents::ControllerEvents() :
	m_pContext(nullptr),
	m_onTransport(nullptr),
	m_refCount(0)
{
}

ControllerEvents::~ControllerEvents()
{
}

IMPLEMENT_IUNKNOWN(ControllerEvents)

void* ControllerEvents::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}

	if (iid == __uuidof(IControllerEvents))
	{
		return dynamic_cast<IControllerEvents*>(this);
	}
	return nullptr;
}

void* ControllerEvents::get_Context()
{
	return m_pContext;
}

void ControllerEvents::put_Context(void* value)
{
	m_pContext = value;
}

ControllerEventHandler ControllerEvents::get_OnCommand()
{
	return m_onTransport;
}
void ControllerEvents::put_OnCommand(ControllerEventHandler value)
{
	m_onTransport = value;
}

void ControllerEvents::ControllerCommand(ControllerCode code)
{
	Invoke(m_onTransport, code);
}

void ControllerEvents::Invoke(ControllerEventHandler handler, ControllerCode code)
{
	void* pContext = m_pContext;

	if (pContext != nullptr && handler != nullptr)
	{
		handler(pContext, code);
	}
}
