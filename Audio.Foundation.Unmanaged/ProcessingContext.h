#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include <AudioTime.h>
using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class _AUDIO_FOUNDATION_UNMANAGED_API ProcessingContext
			{
			public:
				ProcessingContext();
				ProcessingContext(int samplePosition, AudioTime timePosition, bool isSkipping);
				virtual ~ProcessingContext();

				int get_SamplePosition() const;
				void put_SamplePosition(int value);
				_declspec(property(get = get_SamplePosition, put = put_SamplePosition)) int SamplePosition;

				AudioTime get_TimePosition() const;
				void put_TimePosition(AudioTime value);
				_declspec(property(get = get_TimePosition, put = put_TimePosition)) AudioTime TimePosition;

				bool get_IsSkipping() const;
				void put_IsSkipping(bool value);
				_declspec(property(get = get_IsSkipping, put = put_IsSkipping)) bool IsSkipping;

			private:
				int m_samplePosition;
				AudioTime m_timePosition;
				bool m_isSkipping;
			};
		}
	}
}