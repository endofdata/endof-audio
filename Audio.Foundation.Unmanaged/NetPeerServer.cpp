#include "pch.h"
#include "NetPeerServer.h"

using namespace Audio::Foundation::Unmanaged;

NetPeerServer::NetPeerServer(IServerEventsPtr& serverEvents, int bufferSize, bool isUDP, bool isIPv6) :
	NetEndpoint(bufferSize, true, isUDP, isIPv6),
	m_serverEvents(serverEvents),
	m_client(INVALID_SOCKET),
	m_serverThreadId(0),
	m_hServerThread(NULL),
	m_refCount(0)
{
}

NetPeerServer::~NetPeerServer()
{
	ShutDown();
}

IMPLEMENT_IUNKNOWN(NetPeerServer)

void* NetPeerServer::GetInterface(REFIID riid)
{
	if (riid == __uuidof(INetPeerServer))
	{
		return dynamic_cast<INetPeerServer*>(this);
	}
	return NetEndpoint::GetInterface(riid);
}

void NetPeerServer::StartUp()
{
	if (!NetEndpoint::IsOpen)
	{
		throw std::runtime_error("Server is not opened yet.");
	}

	ShutDown();

	m_hServerThread = CreateThread(NULL, 0, NetPeerServer::ServerThreadProc, reinterpret_cast<LPVOID>(this), 0, &m_serverThreadId);
}

void NetPeerServer::ShutDown()
{
	if (m_hServerThread != NULL)
	{
		if (m_client != INVALID_SOCKET)
		{
			shutdown(m_client, SD_SEND);
			// TODO: Recieve pending data?
			closesocket(m_client);

			m_client = INVALID_SOCKET;
		}
		NetEndpoint::Close();

		if (GetCurrentThread() != m_hServerThread)
		{
			WaitForSingleObject(m_hServerThread, INFINITE);
		}
		CloseHandle(m_hServerThread);
		m_hServerThread = NULL;
		m_serverThreadId = 0;
	}
}

DWORD NetPeerServer::ServerThreadProc(LPVOID self)
{
	return reinterpret_cast<NetPeerServer*>(self)->Run();
}

DWORD NetPeerServer::Run()
{
	SOCKADDR_IN clientAddress;
	int addrLen = sizeof(clientAddress);

	DWORD received = 0;
	DWORD sent = 0;
	DWORD flags = 0;

	if (NetEndpoint::IsOpen)
	{
		m_client = WSAAccept(Socket, NULL, 0, NULL, NULL);

		if (m_client == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			HRESULT hr = HRESULT_FROM_WIN32(error);

			ExitThread(hr);
		}
		else
		{
			do
			{
				received = ReceiveFrom(m_client, NetEndpoint::BufferSize);

				if (received > 0)
				{
					int send = OnRequestData(received);

					while (send > 0)
					{
						send = SendTo(m_client, send);

						if (send >= NetEndpoint::BufferSize)
						{
							send = OnContinueResponse();
						}
						else
						{
							break;
						}
					}
				}
			} while (received > 0);
		}
	}
	ExitThread(0);
}


int NetPeerServer::OnRequestData(int avail)
{
	return m_serverEvents->OnRequestData(m_serverEvents->Context, ReceiveBuffer, avail, SendBuffer, NetEndpoint::BufferSize);
}

int NetPeerServer::OnContinueResponse()
{
	return m_serverEvents->OnContinueResponse(m_serverEvents->Context, SendBuffer, NetEndpoint::BufferSize);
}
