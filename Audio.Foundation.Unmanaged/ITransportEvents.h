#pragma once

#include <Audio.Foundation.Unmanaged.h>
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
				enum class TransportCode
				{
					None,
					Stop,
					Start,
					Locate,
					Record					
				};

				typedef void (*TransportEventHandler)(void* pContext, TransportCode code);

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("c192ed5f-7216-4f8e-9348-9376b3d02951")) ITransportEvents : public IUnknown
				{
					/// <summary>
					/// Triggers the OnTransport event
					/// </summary>
					void Transport(TransportCode code) = 0;

					void* get_Context() = 0;
					void put_Context(void* value) = 0;
					/// <summary>
					/// Gets or sets the context parameter for the transport event handler
					/// </summary>
					_declspec(property(get = get_Context, put = put_Context)) void* Context;


					TransportEventHandler get_OnTransport() = 0;
					void put_OnTransport(TransportEventHandler value) = 0;
					/// <summary>
					/// Gets or sets the transport event handler
					/// </summary>
					_declspec(property(get = get_OnTransport, put = put_OnTransport)) TransportEventHandler OnTransport;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ITransportEvents, __uuidof(ITransportEvents));
			}
		}
	}
}