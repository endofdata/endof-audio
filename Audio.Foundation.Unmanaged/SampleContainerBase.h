#pragma once

#include "ISampleBuffer.h"
#include "ISampleContainer.h"
#include <vector>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleContainerBase : public Abstractions::ISampleContainer
			{
			public:
				SampleContainerBase(int sampleCount, int channelCount);
				virtual ~SampleContainerBase();

				virtual int get_SampleCount();
				virtual void put_SampleCount(int sampleCount);

				virtual int get_ChannelCount();
				virtual void put_ChannelCount(int channelCount);

				virtual Abstractions::ISampleBufferPtr get_Channel(int index);

			private:
				void CreateChannels(int sampleCount, int channelCount);

				std::vector<Abstractions::ISampleBufferPtr> m_vecChannels;
				int m_sampleCount;
				bool m_isActive;
			};
		}
	}
}
