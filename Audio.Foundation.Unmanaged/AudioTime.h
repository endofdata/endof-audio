#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <string>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class _AUDIO_FOUNDATION_UNMANAGED_API AudioTime
			{
			public:
				static AudioTime FromSeconds(double seconds);

				AudioTime();
				AudioTime(long long value);

				int get_Minutes() const;
				int get_Seconds() const;
				int get_Milliseconds() const;

				long long get_Value() const;

				operator long long() const;

				std::wstring ToString() const;

				AudioTime& operator + (const AudioTime& other);
				AudioTime& operator - (const AudioTime& other);
				AudioTime& operator += (const AudioTime& other);
				AudioTime& operator -= (const AudioTime& other);

				bool operator == (const AudioTime& other) const;
				bool operator != (const AudioTime& other) const;
				bool operator > (const AudioTime& other) const;
				bool operator < (const AudioTime& other) const;
				bool operator >= (const AudioTime& other) const;
				bool operator <= (const AudioTime& other) const;

			private:
				long long value;
			};

		}
	}
}