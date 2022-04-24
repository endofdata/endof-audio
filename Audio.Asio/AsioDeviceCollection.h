#pragma once

#include "AsioDevice.h"

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class AsioDeviceCollection sealed : public Audio::Foundation::Templates::ReadOnlyCollection<AsioDevice^>
			{
			};
		}
	}
}
