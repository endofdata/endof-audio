#include "pch.h"
#include "DebugController.h"

using namespace Test::Audio::Asio::Unmanaged;

DebugController::DebugController(ITransportPtr& transport) :
	m_refCount(0),
	m_eventHandle(NULL),
	m_transport(transport)
{
	m_transport->Events->Context = this;
	m_transport->Events->OnCommand = OnCommand;
}

DebugController::~DebugController()
{
	put_IsActive(false);
}


IMPLEMENT_IUNKNOWN(DebugController)

void* DebugController::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == __uuidof(IController))
	{
		return dynamic_cast<IController*>(this);
	}
	return nullptr;
}

bool DebugController::get_IsActive() const
{
	return m_eventHandle != NULL;
}

void DebugController::put_IsActive(bool value)
{
	if (value != IsActive)
	{
		if (value == false)
		{
			HANDLE signal = static_cast<HANDLE>(InterlockedExchangePointer(&m_eventHandle, NULL));

			if (signal != NULL)
			{
				CloseHandle(signal);
			}
		}
		else
		{
			HANDLE old_signal = static_cast<HANDLE>(InterlockedExchangePointer(&m_eventHandle, CreateEvent(NULL, TRUE, FALSE, NULL)));

			if (old_signal != NULL)
			{
				CloseHandle(old_signal);
			}
		}
	}
}

bool DebugController::GetNext(unsigned int timeout, ControllerCode& code)
{
	code = ControllerCode::None;

	if (!IsActive)
	{
		return false;
	}

	if(m_queue.empty())
	{
		if (WAIT_TIMEOUT == WaitForSingleObject(m_eventHandle, timeout))
		{
			return false;
		}
	}

	code = m_queue.front();
	m_queue.pop();

	ResetEvent(m_eventHandle);
}

void DebugController::PutNext(ControllerCode code)
{
	m_queue.push(code);
	SetEvent(m_eventHandle);
}

void DebugController::OnCommand(void* pContext, ControllerCode code)
{
	DebugController* self = static_cast<DebugController*>(pContext);

	if (code == ControllerCode::Locate)
	{
		self->m_queue.push(code);
		SetEvent(self->m_eventHandle);
	}
}