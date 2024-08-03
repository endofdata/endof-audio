#pragma once

#include <IController.h>
#include <ITransport.h>
#include "UnknownBase.h"
#include <queue>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Test
{
	namespace Audio
	{
		namespace Asio
		{
			namespace Unmanaged
			{
				class DebugController : public IController
				{
				public:
					DebugController(ITransportPtr& transport);
					virtual ~DebugController();

					bool get_IsActive() const;
					void put_IsActive(bool value);

					bool GetNext(unsigned int timeout, ControllerCode& code);

					void PutNext(ControllerCode code);

					DECLARE_IUNKNOWN

				private:
					static void OnCommand(void* pContext, ControllerCode code);

					ITransportPtr m_transport;
					HANDLE m_eventHandle;
					std::queue<ControllerCode> m_queue;
				};
			}
		}
	}
}