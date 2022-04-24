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

AsioInteropException::AsioInteropException(SerializationInfo^ info, StreamingContext context) :
	Exception(info, context)
{
	int iErrorCode = info->GetInt32("asioError");
	GetAsioErrorFromInt(iErrorCode);
}

void AsioInteropException::GetObjectData(SerializationInfo^ info, StreamingContext context)
{
	Exception::GetObjectData(info, context);

	info->AddValue("asioError", static_cast<Int32>(m_asioError));
}

AsioError AsioInteropException::ASIOError::get()
{
	return m_asioError;
}

void AsioInteropException::GetAsioErrorFromInt(int iErrorCode)
{
	m_asioError = static_cast<AsioError>(iErrorCode);
}

