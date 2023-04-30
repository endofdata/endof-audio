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
	return m_isBypassed;
}

void Oscillator::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}

int Oscillator::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (context.IsLoopStart)
	{
		m_phasePos = 0.0;
	}

	if (!IsBypassed)
	{
		int channelCount = container->ChannelCount;
		int sampleCount = container->SampleCount;
		double phasePos = 0.0;

		for (int c = 0; c < channelCount; c++)
		{
			Sample* pTarget = container->Channels[c]->SamplePtr;
			phasePos = m_phasePos;

			for (int s = 0; s < sampleCount; s++)
			{
				*pTarget++ = std::sin(phasePos) * m_amplitude;
				phasePos += m_phaseStep;
			}
		}
		m_phasePos = std::fmod(phasePos, TwoPi);

		return sampleCount;
	}
	return 0;
}
