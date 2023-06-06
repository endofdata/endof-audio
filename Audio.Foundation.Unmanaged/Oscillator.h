#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleProcessor.h"
#include "IHostClock.h"
#include "IOscillator.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			enum class FadeType
			{
				None = 0,
				FadeIn = 1,
				FadeOut = 2
			};

			class Oscillator : public IOscillator, public ISampleProcessor
			{
			public:
				Oscillator(double sampleRate);
				virtual ~Oscillator();

				double get_Amplitude() const;
				void put_Amplitude(double value);

				double get_Frequency() const;
				void put_Frequency(double value);

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);
				_declspec(property(get = get_IsBypassed, put = put_IsBypassed)) bool IsBypassed;

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				DECLARE_IUNKNOWN

			private:
				double m_sampleRate;
				double m_phasePos;
				double m_phaseStep;
				double m_amplitude;
				double m_frequency;
				bool m_isBypassed;
				double m_preBypassAmplitude;
				double m_fadeStart;
				double m_fadeEnd;
			};
		}
	}
}
