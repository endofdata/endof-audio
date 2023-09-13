#include "pch.h"
#include "NetEndpoint.h"

using namespace Audio::Foundation::Unmanaged;

NetEndpoint::NetEndpoint(int bufferSize, bool isServer, bool isUDP, bool isIPv6) :
	m_socket(INVALID_SOCKET),
	m_isServer(isServer),
	m_isUDP(isUDP),
	m_isIPv6(isIPv6),
	m_bufferSize(0),
	m_refCount(0)
{
	std::memset(&m_sendBuffers, 0, sizeof(WSABUF));
	std::memset(&m_receiveBuffers, 0, sizeof(WSABUF));
	AllocBuffers(bufferSize);
}

NetEndpoint::~NetEndpoint()
{
	Close();
}

IMPLEMENT_IUNKNOWN(NetEndpoint)

void* NetEndpoint::GetInterface(REFIID riid)
{
	if (riid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (riid == __uuidof(INetEndpoint))
	{
		return dynamic_cast<INetEndpoint*>(this);
	}
	return nullptr;
}

bool NetEndpoint::get_IsServer() const
{
	return m_isServer;
}

bool NetEndpoint::get_IsIPv6() const
{
	return m_isIPv6;
}

bool NetEndpoint::get_IsUDP() const
{
	return m_isUDP;
}

int NetEndpoint::get_BufferSize() const
{
	return m_bufferSize;
}

char* NetEndpoint::get_ReceiveBuffer() const
{
	return m_rcvBuf.get();
}

char* NetEndpoint::get_SendBuffer() const
{
	return m_sndBuf.get();
}

bool NetEndpoint::get_IsOpen() const
{
	return m_socket != INVALID_SOCKET;
}

SOCKET NetEndpoint::get_Socket() const
{
	return m_socket;
}

void NetEndpoint::Open(const wchar_t* host, unsigned short port)
{
	Close();
	

	ADDRINFOW* result = GetAddrInfo(host, port);

	try
	{
		m_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		try
		{
			if (IsServer)
			{
				ThrowOn(bind(m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen)), SOCKET_ERROR, WSAGetLastError, "bind");
				ThrowOn(listen(m_socket, SOMAXCONN), SOCKET_ERROR, WSAGetLastError, "listen");
			}
			else
			{
				ThrowOn(connect(m_socket, result->ai_addr, static_cast<int>(result->ai_addrlen)), SOCKET_ERROR, WSAGetLastError, "connect");
			}
		}
		catch (const std::exception&)
		{
			Close();
			throw;
		}

		FreeAddrInfo(result);
	}
	catch (const std::exception&)
	{
		if (result != NULL)
		{
			FreeAddrInfo(result);
		}
		throw;
	}
}

void NetEndpoint::Close()
{
	if (m_socket != INVALID_SOCKET)
	{
		if (!IsServer)
		{
			ThrowOn(shutdown(m_socket, SD_SEND), SOCKET_ERROR, WSAGetLastError, "shutdown");
			// TODO: receive all pending data
		}
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

int NetEndpoint::ReceiveFrom(SOCKET peer, int max)
{
	if (max > BufferSize)
	{
		throw std::invalid_argument("Number of bytes to receive cannot exceed buffer size.");
	}

	DWORD received = 0;
	DWORD flags = 0;

	if (max > 0)
	{
		m_receiveBuffers.len = max;
		ThrowOn(WSARecv(peer, &m_receiveBuffers, 1, &received, &flags, NULL, NULL), SOCKET_ERROR, WSAGetLastError, "WSARecv");
	}
	return static_cast<int>(received);
}

int NetEndpoint::SendTo(SOCKET peer, int send)
{
	if (send > BufferSize)
	{
		throw std::invalid_argument("Number of bytes to send cannot exceed buffer size.");
	}

	DWORD sent = 0;
	DWORD flags = 0;

	if (send > 0)
	{
		m_sendBuffers.len = send;

		ThrowOn(WSASend(peer, &m_sendBuffers, 1, &sent, flags, NULL, NULL), SOCKET_ERROR, WSAGetLastError, "WSASend");
	}
	return static_cast<int>(sent);
}

void NetEndpoint::AllocBuffers(int size)
{
	if (size <= 0)
	{
		throw std::invalid_argument("Buffer size must be greater than zero.");
	}

	m_rcvBuf.reset();
	m_sndBuf.reset();

	if (!IsUDP || IsServer)
	{
		m_rcvBuf = std::make_unique<char[]>(size);
		m_receiveBuffers.len = size;
		m_receiveBuffers.buf = m_rcvBuf.get();
	}

	if (!IsUDP || !IsServer)
	{
		m_sndBuf = std::make_unique<char[]>(size);
		m_sendBuffers.len = size;
		m_sendBuffers.buf = m_sndBuf.get();
	}
	m_bufferSize = size;
}


int NetEndpoint::ThrowOnError(int error, const char* hint)
{
	if (error != 0)
	{
		std::stringstream builder;
		HRESULT hr = HRESULT_FROM_WIN32(error);

		builder << hint << " failed with error 0x" << std::hex << hr << std::dec;
		throw std::runtime_error(builder.str().c_str());
	}
	return error;
}

int NetEndpoint::ThrowOn(int result, int failed, std::function<int()> getError, const char* hint)
{
	if (result == failed)
	{
		ThrowOnError(getError(), hint);
	}
	return result;
}

ADDRINFOW* NetEndpoint::GetAddrInfo(const wchar_t* host, int port)
{
	ADDRINFOW hints;

	std::memset(&hints, 0, sizeof(hints));

	hints.ai_family = IsIPv6 ? AF_INET : AF_INET6;

	if (IsUDP)
	{
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
	}
	else
	{
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
	}
	hints.ai_flags = AI_PASSIVE;

	std::wostringstream builder;

	builder << port;

	ADDRINFOW* result = NULL;

	// Resolve the local address and port to be used by the server
	ThrowOnError(::GetAddrInfo(host, builder.str().c_str(), &hints, &result), "GetAddrInfo");

	return result;
}