#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <NetEndpoint.h>
#include <INetClient.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class NetClient : public NetEndpoint, public INetClient
			{
			public:
				NetClient(int bufferSize = Constants::DefaultNetBufferSize, bool isUDP = false, bool isIPv6 = false);

				int Send(const char* buffer, int size);
				const char* Receive(int& size);

				DECLARE_IUNKNOWN
			};
		}
	}
}