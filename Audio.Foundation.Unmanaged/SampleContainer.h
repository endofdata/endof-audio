#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "SampleContainerSpan.h"
#include "ISampleBuffer.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleContainer : public SampleContainerSpan
			{
			public:
				SampleContainer(int sampleCount, int channelCount);
				virtual ~SampleContainer();

				SampleContainer(std::vector<Sample*>& buffers, int sampleCount);

			protected:
				void CreateChannels(int sampleCount, int channelCount) override;
			};
		}
	}
}
