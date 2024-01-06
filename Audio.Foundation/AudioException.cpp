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

int AudioException::Error::get()
{
	return m_error;
}