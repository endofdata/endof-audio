#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <AudioTime.h>
#include <comdef.h>
#include <MidiMessage.h>
#include <IMidiEvents.h>
#include <mmsystem.h>

using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				typedef bool (*MidiInCapsHandler)(unsigned int deviceId, const MIDIINCAPS& caps);

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("91f6901a-7a86-4754-849b-d3c963750291")) IMidiInput : public IUnknown
				{
					bool Open(int deviceId) = 0;
					void Close() = 0;

					bool Start() = 0;
					bool Stop() = 0;

					IMidiEventsPtr& get_Events() = 0;
					_declspec(property(get = get_Events)) IMidiEventsPtr& Events;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IMidiInput, __uuidof(IMidiInput));
			}
		}
	}
}