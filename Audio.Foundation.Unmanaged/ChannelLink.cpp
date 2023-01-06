#include "pch.h"
#include "ChannelLink.h"
#include "ISampleContainer.h"
#include "ISampleReceiver.h"


using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

ChannelLink::ChannelLink() :
	m_pInput(NULL),
	m_pOutput(NULL),
	m_level(0.0f),
	m_pan(0.0f),
	m_refCount(0)
{
}

ChannelLink::ChannelLink(ISampleContainer* pInput, ISampleReceiver* pOutput, float level, float pan) :
	m_pInput(NULL),
	m_pOutput(NULL),
	m_level(level),
	m_pan(pan),
	m_refCount(0)
{
	put_Input(pInput);
	put_Output(pOutput);
}

ChannelLink::~ChannelLink()
{
	put_Input(NULL);
	put_Output(NULL);
}

IMPLEMENT_IUNKNOWN(ChannelLink)

bool ChannelLink::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(IChannelLink))
	{
		*ppvResult = dynamic_cast<IChannelLink*>(this);
		return true;
	}
	return false;
}

float ChannelLink::get_Level()
{
	return m_level;
}

void ChannelLink::put_Level(float value)
{
	m_level = value;
}

float ChannelLink::get_Pan()
{
	return m_pan;
}

void ChannelLink::put_Pan(float value)
{
	m_pan = value;
}

ISampleContainer* ChannelLink::get_Input()
{
	return m_pInput;
}

void ChannelLink::put_Input(ISampleContainer* value)
{
	if (value != NULL)
	{
		value->AddRef();
	}

	ISampleContainer* pInput = (ISampleContainer*)InterlockedExchangePointer((void**)&m_pInput, value);

	if (pInput != NULL)
	{
		pInput->Release();
	}
}

ISampleReceiver* ChannelLink::get_Output()
{
	return m_pOutput;
}

void ChannelLink::put_Output(ISampleReceiver* value)
{
	if (value != NULL)
	{
		value->AddRef();
	}

	ISampleReceiver* pOutput = (ISampleReceiver*)InterlockedExchangePointer((void**)&m_pOutput, value);

	if (pOutput != NULL)
	{
		pOutput->Release();
	}
}

bool ChannelLink::get_HasInput()
{
	return m_pInput != NULL;
}

bool ChannelLink::get_HasOutput()
{
	return m_pOutput != NULL;
}
