#include "pch.h"
#include "RegisteredDriver.h"

using namespace System;
using namespace Audio::Asio::Interop;

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

