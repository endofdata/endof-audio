#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleProcessor.h"
#include "IHostClock.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class Oscillator : public ISampleProcessor
			{
			public:
				Oscillator(double sampleRate);
				virtual ~Oscillator();

				double get_Amplitude() const;
				void put_Amplitude(double value);
				_declspec(property(get = get_Amplitude, put = put_Amplitude)) double Amplitude;

				double get_Frequency() const;
				void put_Frequency(double value);
				_declspec(property(get = get_Frequency, put = put_Frequency)) double Frequency;

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				DECLARE_IUNKNOWN

			protected:
				bool GetInterface(REFIID riid, void** pResult);

			private:
				double m_sampleRate;
				double m_phasePos;
				double m_phaseStep;
				double m_amplitude;
				double m_frequency;
				bool m_isBypassed;
				bool m_fade;
			};
		}
	}
}
