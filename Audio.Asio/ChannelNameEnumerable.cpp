#include "pch.h"
#include "ChannelNameEnumerable.h"
#include "ChannelNameEnumerator.h"

using namespace System;
using namespace Audio::Asio::Interop;
using namespace System::Collections;
using namespace System::Collections::Generic;

ChannelNameEnumerable::ChannelNameEnumerable(IASIO* pDriver, bool forInput, int numChannels)
{
	if (pDriver == NULL)
	{
		throw gcnew ArgumentNullException("pDriver");
	}
	pDriver->AddRef();
	m_pDriver = pDriver;
	m_isInput = forInput;
	m_numChannels = numChannels;
}

//virtual 
ChannelNameEnumerable::~ChannelNameEnumerable()
{
	CleanUp();
}

ChannelNameEnumerable::!ChannelNameEnumerable()
{
	CleanUp();
}

void ChannelNameEnumerable::CleanUp()
{
	if (m_pDriver != NULL)
	{
		m_pDriver->Release();
		m_pDriver = NULL;
	}
}

System::Collections::Generic::IEnumerator<String^>^ ChannelNameEnumerable::GetEnumerator()
{
	return gcnew ChannelNameEnumerator(m_pDriver, m_isInput, m_numChannels);
}

System::Collections::IEnumerator^ ChannelNameEnumerable::GetNonGenericEnumerator()
{
	return gcnew ChannelNameEnumerator(m_pDriver, m_isInput, m_numChannels);
}