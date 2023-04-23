#include "pch.h"
#include "GainProcessor.h"
#include "SampleConversionUnmanaged.h"
#include <algorithm>
#include <functional>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

GainProcessor::GainProcessor(double level, double pan) :
	m_level(level),
	m_pan(pan),
	m_isBypassed(false),
	m_refCount(0)
{
}

GainProcessor::~GainProcessor()
{
}

IMPLEMENT_IUNKNOWN(GainProcessor)

bool GainProcessor::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleProcessor*>(this));
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	if (iid == __uuidof(ISpatial))
	{
		*ppvResult = dynamic_cast<ISpatial*>(this);
		return true;
	}
	return false;
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
			SampleConversion::LevelAndPanFactor(m_level, m_pan, factorLeft, factorRight);
		}
		else
		{
			factorLeft = factorRight = m_level;
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

double GainProcessor::get_Level() const
{
	return m_level;
}

void GainProcessor::put_Level(double value)
{
	m_level = value;
}

double GainProcessor::get_Pan() const
{
	return m_pan;
}

void GainProcessor::put_Pan(double value)
{
	m_pan = value;
}