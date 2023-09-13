#pragma once

#include <IServerEvents.h>
#include <UnknownBase.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class ServerEvents : public IServerEvents
			{
			public:
				ServerEvents();

				DECLARE_IUNKNOWN;

				void* get_Context() const;
				void put_Context(void* value);

				RequestDataHandler get_OnRequestData() const;
				void put_OnRequestData(RequestDataHandler value);

				ContinueResponseHandler get_OnContinueResponse() const;
				void put_OnContinueResponse(ContinueResponseHandler value);

			private:
				void* m_context;
				RequestDataHandler m_onRequestData;
				ContinueResponseHandler m_onContinueResponse;
			};
		}
	}
}
