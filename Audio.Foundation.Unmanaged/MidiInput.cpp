#include "pch.h"
#include "MidiInput.h"
#include "MidiMessage.h"

using namespace Audio::Foundation::Unmanaged;


MidiInput::MidiInput(IMidiEventsPtr& midiEvents) : 
	m_handle(nullptr),
	m_lastError(MMSYSERR_NOERROR),
	m_events(midiEvents),
	m_refCount(0)
{
}

MidiInput::~MidiInput()
{
	Stop();
	Close();
}

IMPLEMENT_IUNKNOWN(MidiInput)

void* MidiInput::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}

	if (iid == __uuidof(IMidiInput))
	{
		return dynamic_cast<IMidiInput*>(this);
	}
	return nullptr;
}

bool MidiInput::Open(int deviceId)
{
	m_lastError = midiInOpen(&m_handle, (UINT)deviceId, (DWORD_PTR)MidiInput::OnMidiInMsg, (DWORD_PTR)this, CALLBACK_FUNCTION | MIDI_IO_STATUS);

	return m_lastError == MMSYSERR_NOERROR;
}

bool MidiInput::Start()
{
	if (m_handle == NULL)
	{
		return false;
	}

	m_lastError = midiInStart(m_handle);

	return m_lastError == MMSYSERR_NOERROR;
}

bool MidiInput::Stop()
{
	if (m_handle == NULL)
	{
		return false;
	}
	m_lastError = midiInStop(m_handle);

	return m_lastError == MMSYSERR_NOERROR;
}

void MidiInput::Close()
{
	if (m_handle != NULL)
	{
		midiInClose(m_handle);
		m_handle = NULL;
	}
}

IMidiEventsPtr& MidiInput::get_Events()
{
	return m_events;
}

void CALLBACK MidiInput::OnMidiInMsg(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	MidiInput* self = (MidiInput*)dwInstance;

	switch (wMsg)
	{
	case MIM_OPEN:
		self->m_events->Open();
		break;
	case MIM_CLOSE:
		self->m_events->Close();
		break;
	case MIM_DATA:
		self->m_events->Data(MidiMessage(static_cast<unsigned int>(dwParam1)), static_cast<unsigned int>(dwParam2));
		break;
	case MIM_ERROR:
		self->m_events->Error(MidiMessage(static_cast<unsigned int>(dwParam1)), static_cast<unsigned int>(dwParam2));
		break;
	case MIM_MOREDATA:
		self->m_events->Overflow(MidiMessage(static_cast<unsigned int>(dwParam1)), static_cast<unsigned int>(dwParam2));
		break;
		// TODO: Add SysEx support
	case MIM_LONGDATA:
		break;
	case MIM_LONGERROR:
		break;
	}
}

int MidiInput::ListDevices(MidiInCapsHandler handler)
{
	if (handler == nullptr)
	{
		return -1;
	}
	unsigned int numDevs = midiInGetNumDevs();
	MIDIINCAPS devCaps;

	for (unsigned int d = 0; d < numDevs; d++)
	{
		std::memset(&devCaps, 0, sizeof(MIDIINCAPS));
		if (MMSYSERR_NOERROR == midiInGetDevCaps(d, &devCaps, sizeof(devCaps)))
		{
			if (handler(d, devCaps))
			{
				return d;
			}
		}
	}
	return -1;
}
