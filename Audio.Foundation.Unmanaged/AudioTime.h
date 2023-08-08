#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <string>
#include <chrono>

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
				AudioTime(const std::chrono::microseconds& micros);
				AudioTime(const long long value);

				int get_Minutes() const;
				_declspec(property(get = get_Minutes)) int Minutes;

				int get_Seconds() const;
				_declspec(property(get = get_Seconds)) int Seconds;

				int get_Milliseconds() const;
				_declspec(property(get = get_Milliseconds)) int Milliseconds;

				long long get_Value() const;
				void put_Value(long long value);
				_declspec(property(get = get_Value, put = put_Value)) long long Value;

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
				std::lldiv_t m_upper;
				std::lldiv_t m_lower;
				long long m_ticks;
			};

		}
	}
}
