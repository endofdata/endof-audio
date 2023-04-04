#include "pch.h"
#include "MidiEvents.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

MidiEvents::MidiEvents() :
m_pContext(nullptr),
m_onOpen(nullptr),
m_onClose(nullptr),
m_onError(nullptr),
m_onOverflow(nullptr),
m_onData(nullptr),
m_refCount(0)
{
}

MidiEvents::~MidiEvents()
{
}

IMPLEMENT_IUNKNOWN(MidiEvents)

bool MidiEvents::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}

	if (iid == __uuidof(IMidiEvents))
	{
		*ppvResult = dynamic_cast<IMidiEvents*>(this);
		return true;
	}

	*ppvResult = nullptr;
	return false;
}

void* MidiEvents::get_Context()
{
	return m_pContext;
}

void MidiEvents::put_Context(void* value)
{
	m_pContext = value;
}

EventHandler MidiEvents::get_OnOpen()
{
	return m_onOpen;
}
void MidiEvents::put_OnOpen(EventHandler value)
{
	m_onOpen = value;
}

EventHandler MidiEvents::get_OnClose()
{
	return m_onClose;
}
void MidiEvents::put_OnClose(EventHandler value)
{
	m_onClose = value;
}

MessageHandler MidiEvents::get_OnError()
{
	return m_onError;
}
void MidiEvents::put_OnError(MessageHandler value)
{
	m_onError = value;
}

MessageHandler MidiEvents::get_OnOverflow()
{
	return m_onOverflow;
}
void MidiEvents::put_OnOverflow(MessageHandler value)
{
	m_onOverflow = value;
}

MessageHandler MidiEvents::get_OnData()
{
	return m_onData;
}
void MidiEvents::put_OnData(MessageHandler value)
{
	m_onData = value;
}

void MidiEvents::Open()
{
	Invoke(m_onOpen);
}

void MidiEvents::Close()
{
	Invoke(m_onClose);
}

void MidiEvents::Error(const MidiMessage& msg, unsigned int timeStamp)
{
	Invoke(m_onError, msg, timeStamp);
}

void MidiEvents::Overflow(const MidiMessage& msg, unsigned int timeStamp)
{
	Invoke(m_onOverflow, msg, timeStamp);
}

void MidiEvents::Data(const MidiMessage& msg, unsigned int timeStamp)
{
	Invoke(m_onData, msg, timeStamp);
}

void MidiEvents::Invoke(EventHandler handler)
{
	void* pContext = m_pContext;

	if (pContext != nullptr && handler != nullptr)
	{
		handler(pContext);
	}
}

void MidiEvents::Invoke(MessageHandler handler, const MidiMessage& msg, unsigned int timeStamp)
{
	void* pContext = m_pContext;

	if (pContext != nullptr && handler != nullptr)
	{
		handler(pContext, msg, timeStamp);
	}
}
