#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <ISampleReceiver.h>
#include <comdef.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{				
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("519e9d40-db0b-42c4-b435-90d7ec67206a")) ISampleJoiner : public IUnknown
				{
					_declspec(property(get = get_Target, put = put_Target)) ISampleReceiverPtr Target;

					ISampleReceiverPtr get_Target() = 0;
					void put_Target(ISampleReceiverPtr value) = 0;
				};

				_COM_SMARTPTR_TYPEDEF(ISampleJoiner, __uuidof(ISampleJoiner));
			}
		}
	}
}
