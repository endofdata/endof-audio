#include "pch.h"
#include "MidiMessage.h"

using namespace Audio::Foundation::Unmanaged;

MidiMessage::MidiMessage(byte status, byte data1, byte data2) :
	m_status(status),
	m_data1(data1),
	m_data2(data2)
{
}


MidiMessage::MidiMessage(unsigned int value) : 
	MidiMessage((value & 0xFF), ((value >> 8) & 0xFF), ((value >> 16) & 0xFF))
{
}


byte MidiMessage::get_Code() const
{
	return m_status & 0xF0;
}

void MidiMessage::put_Code(byte value)
{
	m_status = m_status & 0x0F | value;
}

byte MidiMessage::get_Channel() const
{
	return m_status & 0x0F;
}

void MidiMessage::put_Channel(byte value)
{
	m_status = m_status & 0xF0 | value;
}

byte MidiMessage::get_Data1() const
{
	return m_data1;
}

void MidiMessage::put_Data1(byte value)
{
	m_data1 = value;
}

byte MidiMessage::get_Data2() const
{
	return m_data2;
}

void MidiMessage::put_Data2(byte value)
{
	m_data2 = value;
}

unsigned int MidiMessage::ToUInt() const
{
	return ((unsigned int)m_status) | ((unsigned int)m_data1 << 8) | ((unsigned int)m_data2 << 16);
}

MidiMessage MidiMessage::NoteOff(byte key, byte velocity)
{
	return MidiMessage(MidiMessage::NOTE_OFF, key, velocity);
}

MidiMessage MidiMessage::NoteOn(byte key, byte velocity)
{
	return MidiMessage(MidiMessage::NOTE_ON, key, velocity);
}

MidiMessage MidiMessage::PolyKeyPressure(byte key, byte velocity)
{
	return MidiMessage(MidiMessage::POLY_KEY_PRESSURE, key, velocity);
}

MidiMessage MidiMessage::ControlChange(byte controller, byte value)
{
	return MidiMessage(MidiMessage::CONTROL_CHANGE, controller, value);
}

MidiMessage MidiMessage::ProgramChange(unsigned short program)
{
	return MidiMessage(MidiMessage::PROGRAM_CHANGE, (program >> 8), (program & 0xFF));
}

MidiMessage MidiMessage::ChannelPressure(unsigned short value)
{
	return MidiMessage(MidiMessage::CHANNEL_PRESSURE, (value >> 8), (value & 0xFF));
}

MidiMessage MidiMessage::PitchBend(unsigned short value)
{
	return MidiMessage(MidiMessage::PITCH_BEND, (value & 0x7F), ((value & 0x3F10) >> 7));
}