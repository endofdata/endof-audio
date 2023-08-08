#pragma once

#include <AudioTime.h>

using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class ManagedAudioTime
			{
			public:
				ManagedAudioTime();
				ManagedAudioTime(ManagedAudioTime^ other);
				ManagedAudioTime(long long micros);
				ManagedAudioTime(const AudioTime& audioTime);

				virtual ~ManagedAudioTime();

				property int Minutes
				{
					int get();
				}

				property int Seconds
				{
					int get();
				}

				property int MilliSeconds
				{
					int get();
				}

				property long long Value
				{
					long long get();
					void set(long long value);
				}

			private:
				System::ValueTuple<long long, long long> _upper;
				System::ValueTuple<long long, long long> _lower;
				long long _ticks;
			};
		}
	}
}
