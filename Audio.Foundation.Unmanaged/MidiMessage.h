#pragma once

#include "Audio.Foundation.Unmanaged.h"

using namespace Audio::Foundation::Unmanaged;

#ifndef byte
typedef unsigned char byte;
#endif

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class _AUDIO_FOUNDATION_UNMANAGED_API MidiMessage
			{
			public:
				static const byte NOTE_OFF = 0x80;
				static const byte NOTE_ON = 0x90;
				static const byte POLY_KEY_PRESSURE = 0xA0;
				static const byte CONTROL_CHANGE = 0xB0;
				static const byte PROGRAM_CHANGE = 0xC0;
				static const byte CHANNEL_PRESSURE = 0xD0;
				static const byte PITCH_BEND = 0xE0;

				MidiMessage(unsigned int value);
				MidiMessage(byte status, byte data1, byte data2);

				byte get_Code() const;
				void put_Code(byte value);

				_declspec(property(get = get_Code, put = put_Code)) byte Code;

				byte get_Channel() const;
				void put_Channel(byte value);

				_declspec(property(get = get_Channel, put = put_Channel)) byte Channel;


				byte get_Data1() const;
				void put_Data1(byte value);

				_declspec(property(get = get_Data1, put = put_Data1)) byte Data1;


				byte get_Data2() const;
				void put_Data2(byte value);

				_declspec(property(get = get_Data2, put = put_Data2)) byte Data2;

				unsigned int ToUInt() const;

				
				static MidiMessage NoteOff(byte key, byte velocity);
				static MidiMessage NoteOn(byte key, byte velocity);
				static MidiMessage PolyKeyPressure(byte key, byte velocity);
				static MidiMessage ControlChange(byte controller, byte value);
				static MidiMessage ProgramChange(unsigned short program);
				static MidiMessage ChannelPressure(unsigned short value);
				static MidiMessage PitchBend(unsigned short value);

			private:
				byte m_status;
				byte m_data1;
				byte m_data2;
			};
		}
	}
}