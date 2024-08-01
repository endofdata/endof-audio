#include "pch.h"
#include "OscControl.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

OscControl::OscControl() :
	m_eventHandle(CreateEvent(nullptr, FALSE, FALSE, nullptr)),
	m_isActive(false),
	m_refCount(0)
{
}

OscControl::~OscControl()
{
}

IMPLEMENT_IUNKNOWN(OscControl)

void* OscControl::GetInterface(REFIID riid)
{
	if (riid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (riid == __uuidof(IController))
	{
		return dynamic_cast<IController*>(this);
	}
	return nullptr;
}

bool OscControl::get_IsActive() const
{
	return m_isActive;
}

void OscControl::put_IsActive(bool value)
{
	if (value != m_isActive)
	{
		if (value)
		{
			m_isActive = true;
		}
		else
		{
			m_isActive = false;
		}
	}
}


bool OscControl::GetNext(unsigned int timeout, ControllerCode& code)
{
	DWORD waitResult = WaitForSingleObject(m_eventHandle, (DWORD)timeout);
	code = ControllerCode::None;

	if (waitResult == WAIT_OBJECT_0)
	{
	}
	return code != ControllerCode::None;
}


