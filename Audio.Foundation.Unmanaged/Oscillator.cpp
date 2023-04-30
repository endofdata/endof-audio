#include "pch.h"
#include "Oscillator.h"

using namespace Audio::Foundation::Unmanaged;

const double TwoPi = 3.14159265358979323846 * 2.0;

Oscillator::Oscillator(double sampleRate) :
	m_sampleRate(sampleRate),
	m_phasePos(0.0),
	m_phaseStep(0.0),
	m_amplitude(1.0),
	m_frequency(440.0),
	m_isBypassed(false),
	m_fade(false),
	m_refCount(0)
{
}

Oscillator::~Oscillator()
{
}

IMPLEMENT_IUNKNOWN(Oscillator)

bool Oscillator::GetInterface(REFIID iid, void** pResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*pResult = reinterpret_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*pResult = reinterpret_cast<ISampleProcessor*>(this);
		return true;
	}
	*pResult = nullptr;
	return false;
}

double Oscillator::get_Amplitude() const
{
	return m_amplitude;
}

void Oscillator::put_Amplitude(double value)
{
	m_amplitude = value;
}

double Oscillator::get_Frequency() const
{
	return m_frequency;
}


void Oscillator::put_Frequency(double value)
{
	m_frequency = value;
	m_phaseStep = value * TwoPi / m_sampleRate;
}

bool Oscillator::get_IsBypassed() const
{
	return m_isBypassed == true && m_fade == false;
}

void Oscillator::put_IsBypassed(bool value)
{
	if (value != m_isBypassed)
	{
		m_fade = true;
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
		double ampChange = 0.0;
		double amplitude = 0.0;

		if (m_fade)
		{
			if (m_isBypassed)
			{
				ampChange = m_amplitude / -sampleCount;
			}
			else
			{
				ampChange = m_amplitude / sampleCount;
				m_amplitude = 0.0;
			}
		}

		for (int c = 0; c < channelCount; c++)
		{
			Sample* pTarget = container->Channels[c]->SamplePtr;
			phasePos = m_phasePos;
			amplitude = m_amplitude;

			for (int s = 0; s < sampleCount; s++)
			{
				*pTarget++ = std::sin(phasePos) * amplitude;
				phasePos += m_phaseStep;
				amplitude += ampChange;
			}
		}
		m_amplitude = amplitude;
		m_fade = false;
		m_phasePos = std::fmod(phasePos, TwoPi);

		return sampleCount;
	}
	return 0;
}
