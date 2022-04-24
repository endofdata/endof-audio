#include "pch.h"
#include "AsioCoreException.h"

using namespace System;
using namespace Audio::Asio;
using namespace System::Runtime::Serialization;

AsioCoreException::AsioCoreException() : Exception()
{
	m_error = E_FAIL;
}

AsioCoreException::AsioCoreException(String^ message) : Exception(message)
{
	m_error = E_FAIL;
}

AsioCoreException::AsioCoreException(String^ message, int code) : Exception(message)
{
	m_error = code;
}

AsioCoreException::AsioCoreException(SerializationInfo^ info, StreamingContext context) : Exception(info, context)
{
	m_error = info->GetInt32("error");
}

void AsioCoreException::GetObjectData(SerializationInfo^ info, StreamingContext context)
{
	Exception::GetObjectData(info, context);

	info->AddValue("error", static_cast<Int32>(m_error));
}

int AsioCoreException::Error::get()
{
	return m_error;
}