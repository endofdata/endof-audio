#include "pch.h"
#include "DriverRegistry.h"
using namespace System;
using namespace System::Collections::ObjectModel;
using namespace Microsoft::Win32;
using namespace Audio::Asio::Interop;

#include "AsioDebugDriverGuid.h"

RegisteredDriver::RegisteredDriver(Guid clsId, String^ name)
{
	_clsId = clsId;
	_name = name;
}

RegisteredDriver::RegisteredDriver(String^ clsIdString, String^ name)
{
	_clsId = Guid::Parse(clsIdString);
	_name = name;
}

Guid RegisteredDriver::ClsId::get()
{
	return _clsId;
}

String^ RegisteredDriver::Name::get()
{
	return _name;
}

// virtual 
String^ RegisteredDriver::ToString()
{
	return _name;
}


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

	asioRoot->Close();

	return result;
}
