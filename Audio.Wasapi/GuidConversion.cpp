#include "pch.h"
#include "GuidConversion.h"

using namespace Audio::Wasapi;

// static 
System::Guid GuidConversion::ToManaged(GUID value)
{
	return System::Guid(System::ReadOnlySpan<System::Byte>(&value, sizeof(GUID)));
}

// static 
GUID GuidConversion::ToUnmanaged(System::Guid value)
{
	return reinterpret_cast<GUID&>(value);
}
