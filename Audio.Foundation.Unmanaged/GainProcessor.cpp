#include "pch.h"
#include "GainProcessor.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>
#include <functional>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

GainProcessor::GainProcessor(double level, double pan) :
	m_isBypassed(false),
	m_refCount(0)
{
}

GainProcessor::~GainProcessor()
{
}

IMPLEMENT_IUNKNOWN(GainProcessor)

void* GainProcessor::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(dynamic_cast<ISampleProcessor*>(this));	}
	if (iid == __uuidof(ISampleProcessor))
	{
		return dynamic_cast<ISampleProcessor*>(this);	}
	if (iid == __uuidof(ISpatial))
	{
		return dynamic_cast<ISpatial*>(this);
	}
	return nullptr;
}

int GainProcessor::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		int channels = container->ChannelCount;
		int sampleCount = container->SampleCount;
		double factorLeft = 0.0;
		double factorRight = 0.0;

		if (channels > 1)
		{
			factorLeft = m_mix.FactorLeft;
			factorRight = m_mix.FactorRight;
		}
		else
		{
			factorLeft = factorRight = m_mix.Level;
		}

		double factor = factorLeft;

		for (int c = 0; c < channels; c++)
		{
			Sample* pSamples = container->Channels[c]->SamplePtr;

			for (int s = 0; s < sampleCount; s++)
			{
				*pSamples++ *= factor;
			}
			factor = factorRight;
		}
	}
	return 0;
}

bool GainProcessor::get_IsBypassed() const
{
	return m_isBypassed;
}

void GainProcessor::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}

MixParameter& GainProcessor::get_Mix()
{
	return m_mix;
}
