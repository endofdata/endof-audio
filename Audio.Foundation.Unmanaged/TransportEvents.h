#pragma once
#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ITransportEvents.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class TransportEvents : public ITransportEvents
			{
			public:
				TransportEvents();
				virtual ~TransportEvents();

				void Transport(TransportCode code);

				void* get_Context();
				void put_Context(void* value);

				TransportEventHandler get_OnTransport();
				void put_OnTransport(TransportEventHandler value);

				DECLARE_IUNKNOWN

			private:
				void Invoke(TransportEventHandler handler, TransportCode code);
	
				void* m_pContext;

				TransportEventHandler m_onTransport;
			};
		}
	}
}
