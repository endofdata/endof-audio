#pragma once

#include "IHostClock.h"
#include "UnknownBase.h"
#include <chrono>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class HostClock : public IHostClock
			{
			public:
				HostClock(double sampleRate);
				virtual ~HostClock();

				AudioTime get_CurrentTime() const;
				void put_CurrentTime(const AudioTime& position);

				void Start();
				void Stop();

				double get_SampleRate() const;
				void put_SampleRate(double value);

				int AudioTimeToSamplePosition(const AudioTime& time) const;
				AudioTime SamplePositonToAudioTime(int samplePosition) const;

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:

				std::chrono::steady_clock::time_point m_offset;
				double m_sampleRate;
			};
		}
	}
}