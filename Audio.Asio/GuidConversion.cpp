#include "pch.h"
#include "GuidConversion.h"

using namespace System;
using namespace Audio::Asio::Interop;

Guid GuidConversion::FromNative(const GUID& guid)
{
	return Guid(
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
}

GUID GuidConversion::FromManaged(Guid guid)
{
	array<Byte>^ guidData = guid.ToByteArray();
	pin_ptr<Byte> data = &(guidData[0]);

	return *(_GUID*)data;
}
