#pragma once
#include <IController.h>
#include <memory>
#include "UnknownBase.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class OscControl : public IController
			{
			public:

				OscControl();
				virtual ~OscControl();

				bool get_IsActive() const;
				void put_IsActive(bool value);
				_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

				bool GetNext(unsigned int timeout, ControllerCode& code);

				DECLARE_IUNKNOWN

			private:
				HANDLE m_eventHandle;
				bool m_isActive;
			};
		}
	}
}