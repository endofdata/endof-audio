#include "pch.h"
#include "SampleJoiner.h"
#include "SampleConversionUnmanaged.h"
#include "IChannelLink.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleJoiner::SampleJoiner(int sampleCount) :
	SampleContainerBase(sampleCount),
	m_pOutputLink(NULL),
	m_refCount(0)
{
}

SampleJoiner::~SampleJoiner()
{
	put_OutputLink(NULL);
}

IMPLEMENT_IUNKNOWN(SampleJoiner)

IMPLEMENT_SAMPLECONTAINER(SampleJoiner)


bool SampleJoiner::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleJoiner*>(this));
		return true;
	}
	if (iid == __uuidof(ISampleJoiner))
	{
		*ppvResult = dynamic_cast<ISampleJoiner*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleReceiver))
	{
		*ppvResult = dynamic_cast<ISampleReceiver*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleContainer))
	{
		*ppvResult = dynamic_cast<ISampleContainer*>(this);
		return true;
	}
	return false;
}


void SampleJoiner::Flush()
{
	LeftChannel->Flush();
	RightChannel->Flush();

	if(NULL != OutputLink)
		OutputLink->Output->Flush();
}

void SampleJoiner::Receive(IChannelLink& receiveLink)
{
	ISampleContainer* pInput = receiveLink.Input;

	if(NULL != pInput)
		MixInput(pInput, receiveLink.Level, receiveLink.Pan);

	Send();
}

void SampleJoiner::Send()
{
	if(NULL != m_pOutputLink)
		m_pOutputLink->Output->Receive(*m_pOutputLink);
}

IChannelLink* SampleJoiner::get_OutputLink()
{
	return m_pOutputLink;
}

void SampleJoiner::put_OutputLink(IChannelLink* value)
{
	if(NULL != value && value->HasOutput == false)
		value = NULL;

	if (value != NULL)
	{
		value->AddRef();
	}

	IChannelLink* pOutputLink = (IChannelLink*)InterlockedExchangePointer((void**)&m_pOutputLink, value);

	if (pOutputLink != NULL)
	{
		pOutputLink->Release();
	}
}

void SampleJoiner::MixInput(ISampleContainer* pInput, float level, float pan)
{
	if (pInput != NULL)
	{
		MixInput(pInput->LeftChannel->SamplePtr, pInput->RightChannel->SamplePtr, level, pan);
	}
}

void SampleJoiner::MixInput(float* pSourceLeft, float* pSourceRight, float level, float pan)
{
	float* pDestLeft = this->LeftChannel->SamplePtr;
	float* pDestRight = this->RightChannel->SamplePtr;

	float lvlPanFactorLeft; 
	float lvlPanFactorRight; 
	SampleConversion::LevelAndPanFactor(level, pan, lvlPanFactorLeft, lvlPanFactorRight);

	for(int i = 0; i < SampleCount; i++)
	{
		*pDestLeft++ = SampleConversion::AddSignals(*pDestLeft, *pSourceLeft++ * lvlPanFactorLeft);
		*pDestRight++ = SampleConversion::AddSignals(*pDestRight, *pSourceRight++ * lvlPanFactorRight);
	}
}
