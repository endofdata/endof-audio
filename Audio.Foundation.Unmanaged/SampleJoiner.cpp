#include "pch.h"
#include "SampleJoiner.h"
#include "SampleConversionUnmanaged.h"

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleJoiner::SampleJoiner(int sampleCount) :
	SampleContainerBase(sampleCount),
	m_pTarget(NULL),
	m_refCount(0)
{
}

SampleJoiner::~SampleJoiner()
{
	put_Target(NULL);
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

	if(NULL != Target)
		Target->Flush();
}

void SampleJoiner::Send()
{
	if(NULL != m_pTarget)
		m_pTarget->Receive(*this);
}

ISampleReceiver* SampleJoiner::get_Target()
{
	return m_pTarget;
}

void SampleJoiner::put_Target(ISampleReceiver* value)
{
	if (value != NULL)
	{
		value->AddRef();
	}

	ISampleReceiver* pTarget = (ISampleReceiver*)InterlockedExchangePointer((void**)&m_pTarget, value);

	if (pTarget != NULL)
	{
		pTarget->Release();
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
