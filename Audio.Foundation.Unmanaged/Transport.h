#pragma once

#include "ITransport.h"
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
			class Transport : public ITransport
			{
			public:
				Transport(IHostClockPtr& hostClock, ITransportEventsPtr& events);
				virtual ~Transport();

				void Start();
				void Stop();

				void Pulse(int sampleCount);

				int get_CurrentSamplePosition() const;
				void put_CurrentSamplePosition(int value);

				bool get_IsSkipping();

				bool get_IsLooping();
				void put_IsLooping(bool value);

				AudioTime get_LoopStart();
				void put_LoopStart(AudioTime value);

				AudioTime get_LoopEnd();
				void put_LoopEnd(AudioTime value);

				IHostClockPtr& get_HostClock();

				ITransportEventsPtr& get_Events();

				int AudioTimeToSamplePosition(const AudioTime& value) const;
				AudioTime SamplePositionToAudioTime(int value) const;

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				IHostClockPtr m_hostClock;
				ITransportEventsPtr m_events;
				int m_currentSamplePosition;
				int m_loopStartPosition;
				int m_loopEndPosition;
				bool m_isLooping;
				bool m_isSkipping;
			};
		}
	}
}