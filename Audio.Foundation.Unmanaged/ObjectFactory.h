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
				static IMeterChannel* CreateMeterChannel(int sampleRate, int channelCount);

				static ISampleBuffer* CreateSampleBuffer(int sampleCount);

				static ISampleJoiner* CreateSampleJoiner(int sampleCount, int channelCount);

				static ISampleContainer* CreateSampleContainer(int sampleCount, int channelCount);

				static ISampleSharer* CreateSampleSharer();

			private:
				ObjectFactory();
			};
		}
	}
}
