#include "pch.h"
#include "NetClient.h"

using namespace Audio::Foundation::Unmanaged;

NetClient::NetClient(int bufferSize, bool isUDP, bool isIPv6) :
	NetEndpoint(bufferSize, false, isUDP, isIPv6),
	m_refCount(0)
{
}

IMPLEMENT_IUNKNOWN(NetClient)

void* NetClient::GetInterface(REFIID riid)
{
	if (riid == __uuidof(INetClient))
	{
		return dynamic_cast<INetClient*>(this);
	}
	return NetEndpoint::GetInterface(riid);
}

int NetClient::Send(const char* buffer, int size)
{
	int completed = 0;

	while (size > 0)
	{
		int send = std::min(size, BufferSize);

		std::memcpy(SendBuffer, buffer, send);
		buffer += send;
		int done = SendTo(Socket, size);
		size -= done;
		completed += done;
	}
	return completed;
}

const char* NetClient::Receive(int& size)
{
	int max = std::min(size, BufferSize);
	size = ReceiveFrom(Socket, max);

	return ReceiveBuffer;
}
