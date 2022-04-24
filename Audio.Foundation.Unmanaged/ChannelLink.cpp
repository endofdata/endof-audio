#include "pch.h"
#include "ChannelLink.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

ChannelLink::ChannelLink() :
	m_pInput(NULL),
	m_pOutput(NULL),
	m_volume(0.0f),
	m_pan(0.0f)
{
}

ChannelLink::ChannelLink(ISampleContainer* pInput, ISampleReceiver* pOutput, float volume, float pan) :
	m_pInput(pInput),
	m_pOutput(pOutput),
	m_volume(volume),
	m_pan(pan)
{
}

ChannelLink::~ChannelLink()
{
}

bool ChannelLink::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IChannelLink))
	{
		*ppvResult = dynamic_cast<IChannelLink*>(this);
		return true;
	}
	return UnknownBase::GetInterface(iid, ppvResult);
}

float ChannelLink::get_Volume()
{
	return m_volume;
}

void ChannelLink::put_Volume(float value)
{
	m_volume = value;
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
	m_pInput = value;
}

ISampleReceiver* ChannelLink::get_Output()
{
	return m_pOutput;
}

void ChannelLink::put_Output(ISampleReceiver* value)
{
	m_pOutput = value;
}
