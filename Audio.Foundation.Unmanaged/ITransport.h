#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <AudioTime.h>
#include <comdef.h>
#include <IHostClock.h>

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
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("de08d184-41e0-4d70-ac8a-5f8a3228fa4b")) ITransport : public IUnknown
				{
					void Start() = 0;
					void Stop() = 0;

					void Pulse() = 0;

					bool get_IsSkipping() = 0;
					_declspec(property(get = get_IsSkipping)) bool IsSkipping;

					bool get_IsLooping() = 0;
					void put_IsLooping(bool value) = 0;
					_declspec(property(get = get_IsLooping, put = put_IsLooping)) bool IsLooping;

					AudioTime get_LoopStart() = 0;
					void put_LoopStart(AudioTime value) = 0;
					_declspec(property(get = get_LoopStart, put = put_LoopStart)) AudioTime LoopStart;

					AudioTime get_LoopEnd() = 0;
					void put_LoopEnd(AudioTime value) = 0;
					_declspec(property(get = get_LoopEnd, put = put_LoopEnd)) AudioTime LoopEnd;

					IHostClockPtr& get_HostClock() = 0;
					_declspec(property(get = get_HostClock)) IHostClockPtr HostClock;
				};


				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ITransport, __uuidof(ITransport));
			}
		}
	}
}