#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "IMidiInput.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class MidiInput : public IMidiInput
			{
			public:
				MidiInput(IMidiEventsPtr& midiEvents);
				virtual ~MidiInput();

				bool Open(int deviceId);
				void Close();

				bool Start();
				bool Stop();

				IMidiEventsPtr& get_Events();

				static int ListDevices(MidiInCapsHandler handler, void* callbackParam = nullptr);

				DECLARE_IUNKNOWN

			private:
				static void CALLBACK OnMidiInMsg(HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);

				IMidiEventsPtr m_events;
				MMRESULT m_lastError;
				HMIDIIN m_handle;
			};
		}
	}
}

