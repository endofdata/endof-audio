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
				HostClock(int sampleRate);
				virtual ~HostClock();

				void Start();
				void Stop();

				AudioTime get_CurrentTime() const;

				int get_SampleRate() const;

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				std::chrono::steady_clock::time_point m_offset;
				int m_sampleRate;
			};
		}
	}
}