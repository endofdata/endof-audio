#pragma once

namespace Audio
{
	namespace Osc
	{
		namespace Unmanaged
		{
			// OSC strings are padded with one to four zero bytes '\0' to make the length a multiple of four

			enum class _AUDIO_OSC_UNMANAGED_API TypeTag
			{
				//i 	int32
				//f 	float32
				//s 	OSC - string
				//b 	OSC - blob

				//h 	64 bit big - endian two’s complement integer
				//t 	OSC - timetag
				//d 	64 bit(“double”) IEEE 754 floating point number
				//S 	Alternate type represented as an OSC - string(for example, for systems that differentiate “symbols” from “strings”)
				//c 	an ascii character, sent as 32 bits
				//r 	32 bit RGBA color
				//m 	4 byte MIDI message.Bytes from MSB to LSB are : port id, status byte, data1, data2
				//T 	True.No bytes are allocated in the argument data.
				//F 	False.No bytes are allocated in the argument data.
				//N 	Nil.No bytes are allocated in the argument data.
				//I 	Infinitum.No bytes are allocated in the argument data.
				//[		Indicates the beginning of an array.The tags following are for data in the Array until a close brace tag is reached.
				//] 	Indicates the end of an array.

				Int32 = 'i',
				Float32 = 'f',
				String = 's',
				Blob = 'b',
				BigEndianIn64 = 'h',
				TimeTag = 't',
				Double = 'd',
				Symbol = 'S',
				Char = 'c',
				RGB = 'r',
				Midi = 'm',
				True = 'T',
				False = 'F',
				Nil = 'N',
				Infinitum = 'I',
				BeginArray = '[',
				EndArray = ']'
			};
		}
	}
}