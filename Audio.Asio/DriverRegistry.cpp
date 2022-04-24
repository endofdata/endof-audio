#include "pch.h"

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
					Guid get()
					{
						return _clsId;
					}
				}

				property String^ Name
				{
					String^ get()
					{
						return _name;
					}
				}

				virtual String^ ToString() override
				{
					return _name;
				}

			internal:
				RegisteredDriver(Guid clsId, String^ name)
				{
					_clsId = clsId;
					_name = name;
				}

				RegisteredDriver(String^ clsIdString, String^ name)
				{
					_clsId = Guid::Parse(clsIdString);
					_name = name;
				}

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

				DriverRegistry()
				{
					LoadFromRegistry();
				}

				virtual Guid GetKeyForItem(RegisteredDriver^ item) override
				{
					return item->ClsId;
				}

				void LoadFromRegistry()
				{
					Clear();

					RegistryKey^ localMachine = Registry::LocalMachine;
					RegistryKey^ asioRoot = localMachine->OpenSubKey("SOFTWARE\\ASIO");
					array<String^>^ subkeyNames = asioRoot->GetSubKeyNames();

					for (int index = 0; index < subkeyNames->Length; index++)
					{
						RegistryKey^ driverKey = asioRoot->OpenSubKey(subkeyNames[index]);
						String^ name = static_cast<String^>(driverKey->GetValue("Description"));
						String^ clsidString = static_cast<String^>(driverKey->GetValue("CLSID"));
					
						// ST: If the description value is not present, use the subkeyname
						if (!name)
							name = subkeyNames[index];

						driverKey->Close();
						Add(gcnew RegisteredDriver(clsidString, name));
					}

					asioRoot->Close();
				}
			};
		}
	}
}
