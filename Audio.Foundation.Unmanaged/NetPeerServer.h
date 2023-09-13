#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <NetEndpoint.h>
#include <functional>
#include <memory>
#include <IServerEvents.h>
#include <INetPeerServer.h>
#include "UnknownBase.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class NetPeerServer : public NetEndpoint, public INetPeerServer
			{
			public:			
				NetPeerServer(IServerEventsPtr& clientEventHandler, int bufferSize = Constants::DefaultNetBufferSize, bool isUDP = false, bool isIPv6 = false);
				virtual ~NetPeerServer();

				void StartUp();
				void ShutDown();

				DECLARE_IUNKNOWN

			private:
				static DWORD ServerThreadProc(LPVOID self);


				IServerEventsPtr m_serverEvents;
				SOCKET m_client;
				DWORD m_serverThreadId;
				HANDLE m_hServerThread;

				DWORD Run();

				int OnRequestData(int avail);
				int OnContinueResponse();
			};
		}
	}
}