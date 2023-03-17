#include "pch.h"
#include "AsioCoreException.h"

using namespace Audio::Asio::Unmanaged;

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
		m_message = pcszMessage;
	}
	else
	{
		m_message = nullptr;
	}
	m_error = code;
}

AsioCoreException::~AsioCoreException()
{
}

int AsioCoreException::get_Error() const
{
	return m_error;
}

const char* AsioCoreException::get_Message() const
{
	return m_message.c_str();
}
