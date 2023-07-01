#pragma once
#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "IControllerEvents.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class ControllerEvents : public IControllerEvents
			{
			public:
				ControllerEvents();
				virtual ~ControllerEvents();

				void ControllerCommand(ControllerCode code);

				void* get_Context();
				void put_Context(void* value);

				ControllerEventHandler get_OnCommand();
				void put_OnCommand(ControllerEventHandler value);

				DECLARE_IUNKNOWN

			private:
				void Invoke(ControllerEventHandler handler, ControllerCode code);
	
				void* m_pContext;

				ControllerEventHandler m_onTransport;
			};
		}
	}
}
