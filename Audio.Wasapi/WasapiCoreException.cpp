#include "pch.h"
#include "WasapiCoreException.h"

using namespace Audio::Wasapi;
using namespace System;
using namespace System::Runtime::Serialization;

WasapiCoreException::WasapiCoreException() : Exception()
{
	m_error = E_FAIL;
}

WasapiCoreException::WasapiCoreException(String^ message) : Exception(message)
{
	m_error = E_FAIL;
}

WasapiCoreException::WasapiCoreException(String^ message, int code) : Exception(message)
{
	m_error = code;
}

int WasapiCoreException::Error::get()
{
	return m_error;
}

// static
void WasapiCoreException::ThrowOnError(HRESULT hr, String^ text)
{
	if (S_OK != hr)
	{
		throw gcnew WasapiCoreException(text, hr);
	}
}
