#include "pch.h"
#include "AudioFoundationException.h"

using namespace Audio::Foundation::Unmanaged;

AudioFoundationException::AudioFoundationException() : AudioFoundationException(nullptr, E_FAIL)
{
}

AudioFoundationException::AudioFoundationException(const char* pcszMessage) : AudioFoundationException(pcszMessage, E_FAIL)
{
}

AudioFoundationException::AudioFoundationException(const char* pcszMessage, int code)
{
	if (pcszMessage != nullptr)
	{
		m_pcszMessage = _strdup(pcszMessage);
	}
	else
	{
		m_pcszMessage = nullptr;
	}
	m_error = code;
}

AudioFoundationException::~AudioFoundationException()
{
	if (m_pcszMessage != nullptr)
	{
		free(m_pcszMessage);
	}
}

int AudioFoundationException::get_Error()
{
	return m_error;
}