#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <WinSock2.h>
#include <functional>
#include <memory>
#include <INetEndpoint.h>
#include "UnknownBase.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class NetEndpoint : public INetEndpoint
			{
			public:
				NetEndpoint(int bufferSize = Constants::DefaultNetBufferSize, bool isServer = false, bool isUDP = false, bool isIPv6 = false);
				virtual ~NetEndpoint();

				bool get_IsServer() const;
				_declspec(property(get = get_IsServer)) bool IsServer;

				bool get_IsUDP() const;
				_declspec(property(get = get_IsUDP)) bool IsUDP;

				bool get_IsIPv6() const;
				_declspec(property(get = get_IsIPv6)) bool IsIPv6;

				int get_BufferSize() const;
				_declspec(property(get = get_BufferSize)) int BufferSize;

				virtual bool get_IsOpen() const;
				_declspec(property(get = get_IsOpen)) bool IsOpen;

				void Open(const wchar_t* host, unsigned short port);
				void Close();

				static void InitWinSock();
				static void ExitWinSock();

				DECLARE_IUNKNOWN

			protected:
				static int ThrowOnError(int error, const char* hint);
				static int ThrowOn(int result, int failed, std::function<int()> getError, const char* hint);

				SOCKET get_Socket() const;
				_declspec(property(get = get_Socket)) SOCKET Socket;

				char* get_ReceiveBuffer() const;
				_declspec(property(get = get_ReceiveBuffer)) char* ReceiveBuffer;

				char* get_SendBuffer() const;
				_declspec(property(get = get_SendBuffer)) char* SendBuffer;

				int ReceiveFrom(SOCKET peer, int max);
				int SendTo(SOCKET peer, int send);

			private:
				ADDRINFOW* GetAddrInfo(const wchar_t* host, int port);
				void AllocBuffers(int size);

				bool m_isServer;
				bool m_isUDP;
				bool m_isIPv6;
				SOCKET m_socket;
				WSABUF m_receiveBuffers;
				WSABUF m_sendBuffers;
				int m_bufferSize;

				std::unique_ptr<char[]> m_rcvBuf;
				std::unique_ptr<char[]> m_sndBuf;
			};
		}
	}
}