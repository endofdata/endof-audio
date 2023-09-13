#include "pch.h"
#include "CppUnitTest.h"

#include <NetPeerServer.h>
#include <NetClient.h>
#include <FoundationObjectFactory.h>
#include <MemCheck.h>
#include <WinsockLifetime.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Audio::Foundation::Unmanaged;
using namespace Test::Audio::Common;

namespace Test
{
	namespace Audio
	{
		namespace Foundation
		{
			namespace Unmanaged
			{
				TEST_CLASS(NetPeerServerTest)
				{
				public:

					const wchar_t* HostName = L"localhost";
					const int Port = 8843;

					TEST_METHOD(CreateClient)
					{
						INetClientPtr client = FoundationObjectFactory::CreateNetClient();
					}

					TEST_METHOD(CreateServerEvents)
					{
						auto events = FoundationObjectFactory::CreateServerEvents();
					}

					TEST_METHOD(CreateServer)
					{
						auto events = FoundationObjectFactory::CreateServerEvents();
						INetPeerServerPtr server = FoundationObjectFactory::CreateNetPeerServer(events);
					}

					TEST_METHOD(InitExitWinSock)
					{
						WinsockLifetime::InitWinSock();
						WinsockLifetime::InitWinSock();
						WinsockLifetime::ExitWinSock();
						WinsockLifetime::ExitWinSock();
					}

					TEST_METHOD(ServerStartStop)
					{
						WinsockLifetime::InitWinSock();
						{
							auto events = FoundationObjectFactory::CreateServerEvents();

							events->Context = this;
							events->OnRequestData = _OnRequestData;
							events->OnContinueResponse = _OnContinueResponse;

							INetPeerServerPtr server = FoundationObjectFactory::CreateNetPeerServer(events);

							((INetEndpointPtr)server)->Open(HostName, Port);
							server->StartUp();
						}
						WinsockLifetime::ExitWinSock();
					}

					TEST_METHOD(SendRequest)
					{
						WinsockLifetime::InitWinSock();
						{
							auto events = FoundationObjectFactory::CreateServerEvents();

							events->Context = this;
							events->OnRequestData = _OnRequestData;
							events->OnContinueResponse = _OnContinueResponse;

							INetPeerServerPtr server = FoundationObjectFactory::CreateNetPeerServer(events);

							((INetEndpointPtr)server)->Open(HostName, Port);
							server->StartUp();

							auto response = SendRequest("Bello?");

							Assert::AreEqual("Banana?", response.c_str(), L"Received a fruty reply.");
						}
						WinsockLifetime::ExitWinSock();
					}

					TEST_METHOD_INITIALIZE(Init)
					{
						m_memCheck.BeginCheck();
					}

					TEST_METHOD_CLEANUP(CleanUp)
					{
						m_memCheck.EndCheck();
					}

				private:
					MemCheck m_memCheck;

					std::string SendRequest(const std::string& text)
					{
						INetClientPtr client = FoundationObjectFactory::CreateNetClient();
						INetEndpointPtr clientEP = client;

						clientEP->Open(HostName, Port);
						client->Send(text.c_str(), static_cast<int>(text.length()));

						int received = clientEP->BufferSize;
						const char* responseData = client->Receive(received);

						return std::string(responseData, received);
					}

					int OnRequestData(const char* requestData, int avail, char* responseData, int maxResponse)
					{
						std::string response("Banana?");

						int copy = std::min(static_cast<int>(response.length()), maxResponse);
						
						std::memcpy(responseData, response.c_str(), copy);

						return copy;
					}

					int OnContinueResponse(char* responseData, int maxResponse)
					{
						return 0;
					}

					static int _OnRequestData(void* context, const char* requestData, int avail, char* responseData, int maxResponse)
					{
						NetPeerServerTest* self = reinterpret_cast<NetPeerServerTest*>(context);

						return self->OnRequestData(requestData, avail, responseData, maxResponse);
					}

					static int _OnContinueResponse(void* context, char* responseData, int maxResponse)
					{
						NetPeerServerTest* self = reinterpret_cast<NetPeerServerTest*>(context);

						return self->OnContinueResponse(responseData, maxResponse);
					}
				};
			}
		}
	}
}
