#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleBuffer.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleBufferSpan : public ISampleBuffer
			{
			public:
				SampleBufferSpan(Sample* pSamples, int sampleCount);

				virtual ~SampleBufferSpan();

				void Clear();

				Sample get_Sample(int iIdx) const;

				void put_Sample(int iIdx, Sample value);

				int get_SampleCount() const;

				Sample* get_SamplePtr();

				int AddTo(ISampleBufferPtr other, int sampleOffset, int sampleCount, int targetOffset) const;

				int CopyTo(ISampleBufferPtr other, int sampleOffset, int sampleCount, int targetOffset) const;

				DECLARE_IUNKNOWN

			protected:
				SampleBufferSpan();

				virtual bool GetInterface(REFIID riid, void** pResult);

				Sample* m_pSamples;
				int m_iSamples;

			private:
				int GetEffectiveCount(int requestedOffset, int requestedCount, int maxCount) const;

			};
		}
	}
}
