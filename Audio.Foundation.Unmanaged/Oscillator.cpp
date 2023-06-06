#include "pch.h"
#include "Oscillator.h"

using namespace Audio::Foundation::Unmanaged;

const double TwoPi = 3.14159265358979323846 * 2.0;

Oscillator::Oscillator(double sampleRate) :
	m_sampleRate(sampleRate),
	m_phasePos(0.0),
	m_phaseStep(0.0),
	m_amplitude(0.0),
	m_frequency(0.0),
	m_isBypassed(false),
	m_preBypassAmplitude(0.0),
	m_fadeStart(0.0),
	m_fadeEnd(0.0),
	m_refCount(0)
{
}

Oscillator::~Oscillator()
{
}

IMPLEMENT_IUNKNOWN(Oscillator)

void* Oscillator::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(dynamic_cast<ISampleProcessor*>(this));
	}
	if (iid == __uuidof(IOscillator))
	{
		return dynamic_cast<IOscillator*>(this);
	}

	if (iid == __uuidof(ISampleProcessor))
	{
		return dynamic_cast<ISampleProcessor*>(this);
	}
	return nullptr;
}

double Oscillator::get_Amplitude() const
{
	return m_amplitude;
}

void Oscillator::put_Amplitude(double value)
{
	if (value == 0.0 && m_amplitude > 0.0)
	{
		m_fadeStart = m_amplitude;
		m_fadeEnd = 0.0;
	}
	else if (m_amplitude == 0.0 && value > 0.0)
	{
		m_fadeStart = 0.0;
		m_fadeEnd = value;
	}
}

double Oscillator::get_Frequency() const
{
	return m_frequency;
}


void Oscillator::put_Frequency(double value)
{
	m_frequency = value;
	m_phaseStep = value * TwoPi / m_sampleRate;

	if (value == 0.0)
	{
		// we always want a 0.0 sample value for freq 0.0, so reset the phase pos
		m_phasePos = 0.0;
	}
}

bool Oscillator::get_IsBypassed() const
{
	return m_isBypassed == true && m_fadeStart == m_fadeEnd;
}

void Oscillator::put_IsBypassed(bool value)
{
	if (value != m_isBypassed)
	{
		if (value)
		{
			m_preBypassAmplitude = m_amplitude;
			Amplitude = 0.0;
		}
		else
		{
			Amplitude = m_preBypassAmplitude;
		}
		m_isBypassed = value;
	}
}

int Oscillator::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!IsBypassed)
	{
		int channelCount = container->ChannelCount;
		int sampleCount = container->SampleCount;
		double phasePos = 0.0;
		double amplitude = 0.0;
		double fadeRange = m_fadeEnd - m_fadeStart;
		double ampChange = fadeRange / sampleCount;
		
		for (int c = 0; c < channelCount; c++)
		{
			Sample* pTarget = container->Channels[c]->SamplePtr;
			phasePos = m_phasePos;
			amplitude = m_fadeStart;

			for (int s = 0; s < sampleCount; s++)
			{
				*pTarget++ = static_cast<Sample>(std::sin(phasePos) * amplitude);
				phasePos += m_phaseStep;
				amplitude += ampChange;
			}
		}
		m_amplitude = m_fadeStart = m_fadeEnd;		
		m_phasePos = std::fmod(phasePos, TwoPi);

		return sampleCount;
	}
	return 0;
}
