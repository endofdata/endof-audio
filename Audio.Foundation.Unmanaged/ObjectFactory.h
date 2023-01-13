#pragma once

#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <ISampleReceiver.h>
#include <ISampleBuffer.h>
#include <ISampleContainer.h>
#include <ISampleJoiner.h>
#include <ISampleSharer.h>
#include <IMeterChannel.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class _AUDIO_FOUNDATION_UNMANAGED_API ObjectFactory
			{
			public:
				static ISampleBufferPtr CreateSampleBuffer(int sampleCount);

				static ISampleContainerPtr CreateSampleContainer(int sampleCount, int channelCount);

				static ISampleSharerPtr CreateSampleSharer();

				static ISampleJoinerPtr CreateSampleJoiner(int sampleCount, int channelCount);

				static IMeterChannelPtr CreateMeterChannel(int sampleRate, int channelCount);

				static ISampleReceiverPtr CreateSampleReceiver(const std::string& filename);

			private:
				ObjectFactory();
			};
		}
	}
}
