#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <AudioTime.h>
#include <comdef.h>

using namespace Audio::Foundation::Unmanaged;

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
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("31f9f535-80da-4b36-9336-ece60fae8384")) IHostClock : public IUnknown
				{
					AudioTime get_CurrentTime() const = 0;
					void put_CurrentTime(const AudioTime& position) = 0;
					_declspec(property(get = get_CurrentTime, put = put_CurrentTime)) AudioTime CurrentTime;

					void Start() = 0;
					void Stop() = 0;

					double get_SampleRate() const = 0;
					void put_SampleRate(double value) = 0;
					_declspec(property(get = get_SampleRate, put = put_SampleRate)) double SampleRate;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IHostClock, __uuidof(IHostClock));
			}
		}
	}
}