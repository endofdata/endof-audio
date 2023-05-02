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

void* MidiEvents::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}

	if (iid == __uuidof(IMidiEvents))
	{
		return dynamic_cast<IMidiEvents*>(this);
	}
	return nullptr;
}

void* MidiEvents::get_Context()
{
	return m_pContext;
}

void MidiEvents::put_Context(void* value)
{
	m_pContext = value;
}

MidiEventHandler MidiEvents::get_OnOpen()
{
	return m_onOpen;
}
void MidiEvents::put_OnOpen(MidiEventHandler value)
{
	m_onOpen = value;
}

MidiEventHandler MidiEvents::get_OnClose()
{
	return m_onClose;
}
void MidiEvents::put_OnClose(MidiEventHandler value)
{
	m_onClose = value;
}

MidiMessageHandler MidiEvents::get_OnError()
{
	return m_onError;
}
void MidiEvents::put_OnError(MidiMessageHandler value)
{
	m_onError = value;
}

MidiMessageHandler MidiEvents::get_OnOverflow()
{
	return m_onOverflow;
}
void MidiEvents::put_OnOverflow(MidiMessageHandler value)
{
	m_onOverflow = value;
}

MidiMessageHandler MidiEvents::get_OnData()
{
	return m_onData;
}
void MidiEvents::put_OnData(MidiMessageHandler value)
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

void MidiEvents::Invoke(MidiEventHandler handler)
{
	void* pContext = m_pContext;

	if (pContext != nullptr && handler != nullptr)
	{
		handler(pContext);
	}
}

void MidiEvents::Invoke(MidiMessageHandler handler, const MidiMessage& msg, unsigned int timeStamp)
{
	void* pContext = m_pContext;

	if (pContext != nullptr && handler != nullptr)
	{
		handler(pContext, msg, timeStamp);
	}
}
