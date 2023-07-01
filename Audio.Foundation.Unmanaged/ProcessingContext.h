#pragma once

#include "Audio.Foundation.Unmanaged.h"
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
				ProcessingContext(int sampleCount);
				virtual ~ProcessingContext();

				int get_SampleCount() const;
				_declspec(property(get = get_SampleCount, put = put_SampleCount)) int SampleCount;

				int get_SamplePosition() const;
				void put_SamplePosition(int value);
				_declspec(property(get = get_SamplePosition, put = put_SamplePosition)) int SamplePosition;

				bool get_IsLoopStart() const;
				void put_IsLoopStart(bool value);
				_declspec(property(get = get_IsLoopStart, put = put_IsLoopStart)) bool IsLoopStart;

				bool get_IsLoopEnd() const;
				void put_IsLoopEnd(bool value);
				_declspec(property(get = get_IsLoopEnd, put = put_IsLoopEnd)) bool IsLoopEnd;

				bool get_IsLooping();
				void put_IsLooping(bool value);
				_declspec(property(get = get_IsLooping, put = put_IsLooping)) bool IsLooping;

				int get_LoopStartSample();
				void put_LoopStartSample(int value);
				_declspec(property(get = get_LoopStartSample, put = put_LoopStartSample)) int LoopStartSample;

				int get_LoopEndSample();
				void put_LoopEndSample(int value);
				_declspec(property(get = get_LoopEndSample, put = put_LoopEndSample)) int LoopEndSample;

				float get_LoopPositionPct();
				_declspec(property(get = get_LoopPositionPct)) float LoopPositionPct;

			private:
				int m_samplePosition;
				bool m_isLoopStart;
				bool m_isLoopEnd;
				int m_loopStartSample;
				int m_loopEndSample;
				bool m_isLooping;
				int m_sampleCount;
			};
		}
	}
}