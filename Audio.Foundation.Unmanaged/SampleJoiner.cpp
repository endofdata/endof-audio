#include "pch.h"
#include "SampleJoiner.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

SampleJoiner::SampleJoiner(int sampleCount, int channelCount) :
	SampleContainerBase(sampleCount, channelCount),
	m_pTarget(NULL),
	m_refCount(0)
{
}

SampleJoiner::~SampleJoiner()
{
}

IMPLEMENT_IUNKNOWN(SampleJoiner)

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
	if (NULL != m_pTarget)
	{
		m_pTarget->Receive(this);
		m_pTarget->Flush();
	}

	for (int c = 0; c < ChannelCount; c++)
	{
		Channels[c]->Flush();
	}
}

ISampleReceiverPtr SampleJoiner::get_Target()
{
	return m_pTarget;
}

void SampleJoiner::put_Target(ISampleReceiverPtr value)
{
	m_pTarget = value;
}

void SampleJoiner::Receive(ISampleContainerPtr container)
{
	int sourceChannels = container == nullptr? 0 : container->ChannelCount;
	int targetChannels = ChannelCount;
	float chnLvl = 1.0f;

	for (int c = 0; c < targetChannels; c++)
	{
		float* pDest = Channels[c]->SamplePtr;

		if (c < sourceChannels)
		{
			const float* pSource = container->Channels[c]->SamplePtr;

			for (int i = 0; i < SampleCount; i++)
			{
				*pDest++ = SampleConversion::AddSignals(*pDest, *pSource++ * chnLvl);
			}
		}
		else
		{
			ZeroMemory(pDest, SampleCount * sizeof(float));
		}
	}
}
