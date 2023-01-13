#include "pch.h"
#include "AsioCoreException.h"

using namespace Audio::Asio;

AsioCoreException::AsioCoreException() : AsioCoreException(NULL, E_FAIL)
{
}

AsioCoreException::AsioCoreException(const char* pcszMessage) : AsioCoreException(pcszMessage, E_FAIL)
{
}

AsioCoreException::AsioCoreException(const char* pcszMessage, int code)
{
	if (pcszMessage != NULL)
	{
		m_pcszMessage = _strdup(pcszMessage);
	}
	else
	{
		m_pcszMessage = NULL;
	}
	m_error = code;
}

AsioCoreException::~AsioCoreException()
{
	if (m_pcszMessage != NULL)
	{
		free(m_pcszMessage);
	}
}

int AsioCoreException::get_Error()
{
	return m_error;
}