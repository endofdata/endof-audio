#include "pch.h"
#include "MidiTransportControl.h"
#include <FoundationObjectFactory.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

MidiTransportControl::MidiTransportControl(IMidiInputPtr& midiInput, ITransportPtr& transport) :
	m_input(midiInput),
	m_transport(transport),
	m_eventHandle(CreateEvent(nullptr, FALSE, FALSE, nullptr)),
	m_code(TransportCode::None),
	m_isActive(false),
	m_refCount(0)
{
	m_input->Events->Context = this;
	m_input->Events->OnData = OnData;
	m_transport->Events->Context = this;
	m_transport->Events->OnTransport = OnTransport;
}

MidiTransportControl::~MidiTransportControl()
{
}

IMPLEMENT_IUNKNOWN(MidiTransportControl)

void* MidiTransportControl::GetInterface(REFIID riid)
{
	if (riid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (riid == __uuidof(ITransportControl))
	{
		return dynamic_cast<ITransportControl*>(this);
	}
	return nullptr;
}

bool MidiTransportControl::get_IsActive() const
{
	return m_isActive;
}

void MidiTransportControl::put_IsActive(bool value)
{
	if (value != m_isActive)
	{
		if (value)
		{
			m_isActive = m_input->Start();
		}
		else
		{
			m_input->Stop();
			m_isActive = false;
		}
	}
}


bool MidiTransportControl::GetNext(unsigned int timeout, TransportCode& code)
{
	DWORD waitResult = WaitForSingleObject(m_eventHandle, (DWORD)timeout);
	code = TransportCode::None;

	if(waitResult == WAIT_OBJECT_0)
	{
		code = m_code;
	}
	return code != TransportCode::None;
}

void MidiTransportControl::OnData(void* pContext, const MidiMessage& msg, unsigned int timeStamp)
{
	MidiTransportControl* self = static_cast<MidiTransportControl*>(pContext);

	if (msg.Code == MidiMessage::CONTROL_CHANGE && msg.Data2 > 0)
	{
		TransportCode code = TransportCode::None;

		// TODO: Make MIDI control codes configurable
		switch (msg.Data1)
		{
		case 70:
			code = TransportCode::Locate;
			break;
		case 71:
			code = TransportCode::Stop;
			break;
		case 73:
			code = TransportCode::Start;
			break;
		case 74:
			code = TransportCode::Record;
			break;
		}

		if (code != TransportCode::None)
		{
			self->m_code = code;
			SetEvent(self->m_eventHandle);
		}
	}
}

void MidiTransportControl::OnTransport(void* pContext, TransportCode code)
{
	MidiTransportControl* self = static_cast<MidiTransportControl*>(pContext);

	if (code == TransportCode::Locate)
	{
		self->m_code = code;
		SetEvent(self->m_eventHandle);
	}
}

