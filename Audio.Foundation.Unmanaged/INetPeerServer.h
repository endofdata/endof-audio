#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <INetEndpoint.h>

using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("945b1d84-04cb-4f41-8f91-310766b22965")) INetPeerServer : public IUnknown
				{
					void StartUp() = 0;
					void ShutDown() = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(INetPeerServer, __uuidof(INetPeerServer));
			}
		}
	}
}