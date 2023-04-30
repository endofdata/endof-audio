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
				Transport(IHostClockPtr& hostClock, ITransportEventsPtr& events, int sampleCount);
				virtual ~Transport();

				void Start();
				void Stop();

				ProcessingContext& Pulse();

				ProcessingContext& get_Context();

				AudioTime get_TimePosition() const;
				void put_TimePosition(AudioTime value);

				AudioTime get_LoopStartTime();
				void put_LoopStartTime(AudioTime value);

				AudioTime get_LoopEndTime();
				void put_LoopEndTime(AudioTime value);

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
				ProcessingContext m_context;
			};
		}
	}
}