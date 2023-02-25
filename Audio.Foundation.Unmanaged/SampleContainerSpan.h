#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleContainer.h"
#include "ISampleBuffer.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleContainerSpan : public ISampleContainer
			{
			public:
				SampleContainerSpan(std::vector<ISampleBufferPtr> vecChannels, int sampleCount = 0);
				virtual ~SampleContainerSpan();

				virtual int get_SampleCount() const;
				virtual void put_SampleCount(int sampleCount);

				virtual int get_ChannelCount() const;
				virtual void put_ChannelCount(int channelCount);

				virtual ISampleBufferPtr get_Channel(int index);

				virtual int AddTo(ISampleContainerPtr other, int sampleOffset, int sampleCount, int channelOffset, int channelCount, int targetSampleOffset, int targetChannelOffset) const;

				virtual int CopyTo(ISampleContainerPtr other, int sampleOffset, int sampleCount, int channelOffset, int channelCount, int targetSampleOffset, int targetChannelOffset) const;

				virtual ISampleContainerPtr Span(int sampleOffset, int sampleCount, int channelOffset, int channelCount);

				DECLARE_IUNKNOWN

			protected:
				SampleContainerSpan();

				virtual bool GetInterface(REFIID riid, void** pResult);

				std::vector<ISampleBufferPtr> m_vecChannels;
				int m_sampleCount;
				bool m_isActive;
			};
		}
	}
}
