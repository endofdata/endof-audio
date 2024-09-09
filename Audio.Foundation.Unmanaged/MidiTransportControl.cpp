#include "pch.h"
#include "MidiTransportControl.h"
#include <FoundationObjectFactory.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

MidiTransportControl::MidiTransportControl(IMidiInputPtr& midiInput, ITransportPtr& transport) :
	m_input(midiInput),
	m_transport(transport),
	m_eventHandle(CreateEvent(nullptr, FALSE, FALSE, nullptr)),
	m_code(ControllerCode::None),
	m_isActive(false),
	m_refCount(0)
{
	m_input->Events->Context = this;
	m_input->Events->OnData = OnData;
	m_transport->Events->Context = this;
	m_transport->Events->OnCommand = OnCommand;
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
	if (riid == __uuidof(IController))
	{
		return dynamic_cast<IController*>(this);
	}
	return nullptr;
}

void MidiTransportControl::ConfigureMappings(const ControllerMapping mappings[], size_t count)
{
	m_mappings.clear();

	for (size_t index = 0; index < count; index++)
	{
		const ControllerMapping& mapping = mappings[index];
		m_mappings.push_back(std::pair<unsigned char, ControllerCode>(static_cast<unsigned char>(mapping.CommandId), mapping.Code));
	}
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


bool MidiTransportControl::GetNext(unsigned int timeout, ControllerCode& code)
{
	DWORD waitResult = WaitForSingleObject(m_eventHandle, (DWORD)timeout);
	code = ControllerCode::None;

	if(waitResult == WAIT_OBJECT_0)
	{
		code = m_code;
	}
	return code != ControllerCode::None;
}

void MidiTransportControl::OnData(void* pContext, const MidiMessage& msg, unsigned int timeStamp)
{
	MidiTransportControl* self = static_cast<MidiTransportControl*>(pContext);

	if (msg.Code == MidiMessage::CONTROL_CHANGE && msg.Data2 > 0)
	{
		unsigned char data1 = msg.Data1;
		auto mapping = std::find_if(self->m_mappings.begin(), self->m_mappings.end(), 
			[data1](std::pair<unsigned char, ControllerCode> pair) { return pair.first == data1; });

		if (mapping != self->m_mappings.end())
		{
			self->m_code = mapping->second;
			SetEvent(self->m_eventHandle);
		}
	}
}

void MidiTransportControl::OnCommand(void* pContext, ControllerCode code)
{
	MidiTransportControl* self = static_cast<MidiTransportControl*>(pContext);

	if (code == ControllerCode::Locate)
	{
		self->m_code = code;
		SetEvent(self->m_eventHandle);
	}
}

