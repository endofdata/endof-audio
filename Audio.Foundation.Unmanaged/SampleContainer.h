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
			class SampleContainer : public ISampleContainer
			{
			public:
				SampleContainer(int sampleCount, int channelCount);
				virtual ~SampleContainer();

				virtual int get_SampleCount();
				virtual void put_SampleCount(int sampleCount);

				virtual int get_ChannelCount();
				virtual void put_ChannelCount(int channelCount);

				virtual ISampleBufferPtr get_Channel(int index);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				void CreateChannels(int sampleCount, int channelCount);

				std::vector<ISampleBufferPtr> m_vecChannels;
				int m_sampleCount;
				bool m_isActive;
			};
		}
	}
}
