#include "pch.h"
#include "SampleJoiner.h"
#include "SampleConversionUnmanaged.h"
#include "IChannelLink.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleJoiner::SampleJoiner(int sampleCount) :
	SampleContainer(sampleCount),
	m_pOutputLink(NULL)
{
}

SampleJoiner::~SampleJoiner()
{
}

bool SampleJoiner::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(ISampleJoiner))
	{
		*ppvResult = dynamic_cast<ISampleJoiner*>(this);
		return true;
	}
	return SampleContainer::GetInterface(iid, ppvResult);
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
		MixInput(pInput, receiveLink.Volume, receiveLink.Pan);

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
	if(NULL != value && value->Output == NULL)
		value = NULL;

	m_pOutputLink = value;
}

void SampleJoiner::MixInput(ISampleContainer* pInput, float volume, float pan)
{
	MixInput(pInput->LeftChannel->SamplePtr, pInput->RightChannel->SamplePtr, volume, pan);
}

void SampleJoiner::MixInput(float* pSourceLeft, float* pSourceRight, float volume, float pan)
{
	float* pDestLeft = this->LeftChannel->SamplePtr;
	float* pDestRight = this->RightChannel->SamplePtr;

	float volPanFactorLeft; 
	float volPanFactorRight; 
	SampleConversion::VolumeAndPanFactor(volume, pan, volPanFactorLeft, volPanFactorRight);

	for(int i = 0; i < SampleCount; i++)
	{
		*pDestLeft++ = SampleConversion::AddSignals(*pDestLeft, *pSourceLeft++ * volPanFactorLeft);
		*pDestRight++ = SampleConversion::AddSignals(*pDestRight, *pSourceRight++ * volPanFactorRight);
	}
}
