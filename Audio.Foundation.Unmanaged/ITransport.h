#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <AudioTime.h>
#include <comdef.h>
#include <IHostClock.h>
#include <IControllerEvents.h>
#include <ProcessingContext.h>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				/// <summary>
				/// AudioTime information from host
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("de08d184-41e0-4d70-ac8a-5f8a3228fa4b")) ITransport : public IUnknown
				{
					void Run() = 0;
					void Stop() = 0;

					ProcessingContext& Pulse() = 0;

					ProcessingContext& get_Context() = 0;
					_declspec(property(get = get_Context)) ProcessingContext& Context;

					AudioTime get_TimePosition() const = 0;
					void put_TimePosition(AudioTime value) = 0;
					_declspec(property(get = get_TimePosition, put = put_TimePosition)) AudioTime TimePosition;

					AudioTime get_LoopStartTime() = 0;
					void put_LoopStartTime(AudioTime value) = 0;
					_declspec(property(get = get_LoopStartTime, put = put_LoopStartTime)) AudioTime LoopStartTime;

					AudioTime get_LoopEndTime() = 0;
					void put_LoopEndTime(AudioTime value) = 0;
					_declspec(property(get = get_LoopEndTime, put = put_LoopEndTime)) AudioTime LoopEndTime;

					IHostClockPtr& get_HostClock() = 0;
					_declspec(property(get = get_HostClock)) IHostClockPtr HostClock;

					IControllerEventsPtr& get_Events() = 0;
					_declspec(property(get = get_Events)) IControllerEventsPtr& Events;

					int AudioTimeToSamplePosition(const AudioTime& value) const = 0;
					AudioTime SamplePositionToAudioTime(int value) const = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ITransport, __uuidof(ITransport));
			}
		}
	}
}