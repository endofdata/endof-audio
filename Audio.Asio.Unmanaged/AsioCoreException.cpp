#include "pch.h"
#include "AsioCoreException.h"

using namespace Audio::Asio;

AsioCoreException::AsioCoreException() : AsioCoreException(nullptr, E_FAIL)
{
}

AsioCoreException::AsioCoreException(const char* pcszMessage) : AsioCoreException(pcszMessage, E_FAIL)
{
}

AsioCoreException::AsioCoreException(const char* pcszMessage, int code)
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

AsioCoreException::~AsioCoreException()
{
	if (m_pcszMessage != nullptr)
	{
		free(m_pcszMessage);
	}
}

int AsioCoreException::get_Error()
{
	return m_error;
}