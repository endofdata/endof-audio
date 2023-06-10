#include "pch.h"
#define INITGUID
#include "AsioDebugDriverGuid.h"
#undef INITGUID
#include "DriverRegistry.h"
using namespace System;
using namespace System::Collections::ObjectModel;
using namespace Microsoft::Win32;
using namespace Audio::Asio::Interop;



DriverRegistry::DriverRegistry()
{
}

// virtual 
Guid DriverRegistry::GetKeyForItem(RegisteredDriver^ item)
{
	return item->ClsId;
}

// static 
DriverRegistry^ DriverRegistry::FromRegistry()
{
	DriverRegistry^ result = gcnew DriverRegistry();

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
		result->Add(gcnew RegisteredDriver(clsidString, name));
	}

#if _DEBUG
	ReadOnlySpan<byte> span((unsigned char*)&CLSID_AsioDebugDriver, sizeof(GUID));

	result->Add(gcnew RegisteredDriver(Guid::Guid(span).ToString(), "ASIO Debug Driver"));
#endif

	asioRoot->Close();

	return result;
}
