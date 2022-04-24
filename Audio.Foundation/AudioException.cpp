#include "pch.h"
#include "AudioException.h"

using namespace System;
using namespace Audio::Foundation::Interop;
using namespace System::Runtime::Serialization;

AudioException::AudioException() : Exception()
{
	m_error = E_FAIL;
}

AudioException::AudioException(String^ message) : Exception(message)
{
	m_error = E_FAIL;
}

AudioException::AudioException(String^ message, int code) : Exception(message)
{
	m_error = code;
}

AudioException::AudioException(SerializationInfo^ info, StreamingContext context) : Exception(info, context)
{
	m_error = info->GetInt32("error");
}

void AudioException::GetObjectData(SerializationInfo^ info, StreamingContext context)
{
	Exception::GetObjectData(info, context);

	info->AddValue("error", static_cast<Int32>(m_error));
}

int AudioException::Error::get()
{
	return m_error;
}