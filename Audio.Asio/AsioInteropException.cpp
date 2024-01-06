#include "pch.h"
#include "AsioInteropException.h"

using namespace System;
using namespace Audio::Asio::Interop;
using namespace System::Runtime::Serialization;

AsioInteropException::AsioInteropException(String^ message, System::Runtime::InteropServices::SEHException^ sehException) :
	Exception(message, sehException)
{
	GetAsioErrorFromInt(sehException->ErrorCode);
}

AsioInteropException::AsioInteropException(String^ message, int iErrorCode) :
	Exception(message)
{
	GetAsioErrorFromInt(iErrorCode);
}

Audio::Asio::Unmanaged::AsioError AsioInteropException::ASIOError::get()
{
	return m_asioError;
}

void AsioInteropException::GetAsioErrorFromInt(int iErrorCode)
{
	m_asioError = static_cast<Audio::Asio::Unmanaged::AsioError>(iErrorCode);
}

