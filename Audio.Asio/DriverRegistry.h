#pragma once

#include "AsioDebugDriverGuid.h"
#include "RegisteredDriver.h"

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class DriverRegistry : System::Collections::ObjectModel::KeyedCollection<System::Guid, RegisteredDriver^>
			{
			public:
				static System::Guid DebugDriverGuid = System::Guid(CLSID_AsioDebugDriver.Data1, CLSID_AsioDebugDriver.Data2, CLSID_AsioDebugDriver.Data3,
					CLSID_AsioDebugDriver.Data4[0], CLSID_AsioDebugDriver.Data4[1], CLSID_AsioDebugDriver.Data4[2], CLSID_AsioDebugDriver.Data4[3],
					CLSID_AsioDebugDriver.Data4[4], CLSID_AsioDebugDriver.Data4[5], CLSID_AsioDebugDriver.Data4[6], CLSID_AsioDebugDriver.Data4[7]);

				DriverRegistry();

				virtual System::Guid GetKeyForItem(RegisteredDriver^ item) override;

				static DriverRegistry^ FromRegistry();
			};
		}
	}
}
