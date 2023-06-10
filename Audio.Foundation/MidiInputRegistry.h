#pragma once

#include "RegisteredMidiInput.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class MidiInputRegistry : System::Collections::ObjectModel::KeyedCollection<unsigned int, RegisteredMidiInput^>
			{
			public:
				MidiInputRegistry();

				virtual unsigned int GetKeyForItem(RegisteredMidiInput^ item) override;

				static MidiInputRegistry^ FromMmSystem();

			private:
				static bool MidiInCapsHandler(unsigned int deviceId, const MIDIINCAPS& caps, void* callbackParam);
			};
		}
	}
}
