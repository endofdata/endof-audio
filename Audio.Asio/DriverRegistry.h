#pragma once

using namespace System;
using namespace System::Collections::ObjectModel;
using namespace Microsoft::Win32;

#include "AsioDebugDriverGuid.h"

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class RegisteredDriver sealed
			{
			public:
				property Guid ClsId
				{
					Guid get();
				}

				property String^ Name
				{
					String^ get();
				}

				virtual String^ ToString() override;

			internal:
				RegisteredDriver(Guid clsId, String^ name);

				RegisteredDriver(String^ clsIdString, String^ name);

			private:
				Guid _clsId;
				String^ _name;
			};

			public ref class DriverRegistry : KeyedCollection<Guid, RegisteredDriver^>
			{
			public:
				static Guid DebugDriverGuid = Guid(CLSID_AsioDebugDriver.Data1, CLSID_AsioDebugDriver.Data2, CLSID_AsioDebugDriver.Data3,
					CLSID_AsioDebugDriver.Data4[0], CLSID_AsioDebugDriver.Data4[1], CLSID_AsioDebugDriver.Data4[2], CLSID_AsioDebugDriver.Data4[3],
					CLSID_AsioDebugDriver.Data4[4], CLSID_AsioDebugDriver.Data4[5], CLSID_AsioDebugDriver.Data4[6], CLSID_AsioDebugDriver.Data4[7]);

				DriverRegistry();

				virtual Guid GetKeyForItem(RegisteredDriver^ item) override;

				static DriverRegistry^ FromRegistry();
			};
		}
	}
}
#pragma once
