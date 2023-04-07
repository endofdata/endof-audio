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

				void Pulse();

				bool get_IsSkipping();

				bool get_IsLooping();
				void put_IsLooping(bool value);

				AudioTime get_LoopStart();
				void put_LoopStart(AudioTime value);

				AudioTime get_LoopEnd();
				void put_LoopEnd(AudioTime value);

				IHostClockPtr& get_HostClock();

				ITransportEventsPtr& get_Events();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				IHostClockPtr m_hostClock;
				ITransportEventsPtr m_events;
				AudioTime m_loopStart;
				AudioTime m_loopEnd;
				bool m_isLooping;
				bool m_isSkipping;
			};
		}
	}
}