#include "pch.h"
#include "ChannelNameEnumerator.h"
#include "AsioError.h"
#include "AsioCoreException.h"

using namespace System;
using namespace Audio::Asio::Interop;

ChannelNameEnumerator::ChannelNameEnumerator(IASIO* pDriver, bool forInput, int numChannels)
{
	if (pDriver == NULL)
	{
		throw gcnew ArgumentNullException("pDriver");
	}
	pDriver->AddRef();
	m_pDriver = pDriver;
	m_isInput = forInput;
	m_maxId = numChannels - 1;
	m_currentName = nullptr;
	m_currentId = -1;
}

//virtual 
ChannelNameEnumerator::~ChannelNameEnumerator()
{
	CleanUp();
}

ChannelNameEnumerator::!ChannelNameEnumerator()
{
	CleanUp();
}

void ChannelNameEnumerator::CleanUp() 
{
	if (m_pDriver != NULL)
	{
		m_pDriver->Release();
		m_pDriver = NULL;
	}
}

//virtual 
String^ ChannelNameEnumerator::Current::get()
{
	return m_currentName;
}

//virtual 
Object^ ChannelNameEnumerator::NonGenericCurrent::get()
{
	return m_currentName;
}

//virtual 
bool ChannelNameEnumerator::MoveNext()
{
	if (m_currentId >= m_maxId)
	{
		return false;
	}
	ASIOChannelInfo channelInfo;

	channelInfo.isInput = m_isInput? ASIOTrue : ASIOFalse;
	channelInfo.channel = ++m_currentId;

	ThrowIfFailed(m_pDriver->getChannelInfo(&channelInfo));

	m_currentName = gcnew String(channelInfo.name);
	return true;
}

// virtual
void ChannelNameEnumerator::Reset()
{
	m_currentId = -1;
	m_currentName = nullptr;
}

void ChannelNameEnumerator::ThrowIfFailed(ASIOError error)
{
	if (ASE_OK != error)
	{
		// Cast does not fail: If error is a known Interop::AsioError enum value, we see the corresponding value, otherwise, the numeric error value.
		Audio::Asio::Interop::AsioError errorValue = (Audio::Asio::Interop::AsioError)error;

		throw gcnew AsioCoreException(System::String::Format(L"AsioCore: ASIO driver reported error '{0}'.", errorValue), (int)error);
	}
}