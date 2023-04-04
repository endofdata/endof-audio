#include "pch.h"
#include "TransportControl.h"
#include <ObjectFactory.h>

using namespace RepeatMyself;

TransportControl::TransportControl(IMidiInputPtr& midiInput) :
	m_input(midiInput),
	m_eventHandle(CreateEvent(nullptr, FALSE, FALSE, nullptr)),
	m_code(TransportCode::None),
	m_isActive(false)
{
	m_input->Events->Context = this;
	m_input->Events->OnData = OnData;
}

TransportControl::~TransportControl()
{
}

bool TransportControl::get_IsActive() const
{
	return m_isActive;
}

void TransportControl::put_IsActive(bool value)
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


bool TransportControl::GetNext(unsigned int timeout, TransportCode& code)
{
	DWORD waitResult = WaitForSingleObject(m_eventHandle, (DWORD)timeout);
	code = TransportCode::None;

	if(waitResult == WAIT_OBJECT_0)
	{
		code = m_code;
	}
	return code != TransportCode::None;
}

void TransportControl::OnData(void* pContext, const MidiMessage& msg, unsigned int timeStamp)
{
	TransportControl* self = static_cast<TransportControl*>(pContext);

	if (msg.Code == MidiMessage::CONTROL_CHANGE)
	{
		TransportCode code = TransportCode::None;

		switch (msg.Data1)
		{
		case 70:
			code = TransportCode::ToStart;
			break;
		case 71:
			code = TransportCode::Stop;
			break;
		case 73:
			code = TransportCode::Play;
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

bool onDeviceCaps(unsigned int id, const MIDIINCAPS& devcaps)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utf16conv;
	auto name = utf16conv.to_bytes(devcaps.szPname);

	std::cout << "Input ID " << id << " '" << name.c_str() << "'" << std::endl;

	//bool isSelected = name == "2- Steinberg UR-RT2-1";
	bool isSelected = name == "LoopBe Internal MIDI";

	if (isSelected)
	{
		std::cout << "Selected MIDI input device ID '" << id  << "'" << std::endl;
	}
	return isSelected;
}

TransportControlPtr TransportControl::Create()
{
	IMidiInputPtr midiInput = ObjectFactory::CreateMidiInput();
	int midiDevId = ObjectFactory::SelectMidiInputDevice(onDeviceCaps);

	if (midiDevId >= 0 && midiInput->Open(midiDevId))
	{
		return TransportControlPtr(new TransportControl(midiInput));
	}
	return nullptr;
}