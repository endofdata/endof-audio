#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <IControllerEvents.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				struct _AUDIO_FOUNDATION_UNMANAGED_API ControllerMapping
				{
				public:
					ControllerMapping(ControllerCode code, int commandId);

					ControllerCode get_Code() const;
					_declspec(property(get = get_Code)) ControllerCode Code;

					int get_CommandId() const;
					_declspec(property(get = get_CommandId)) int CommandId;

				private:
					int m_commandId;
					ControllerCode m_code;
				};
			}
		}
	}
}