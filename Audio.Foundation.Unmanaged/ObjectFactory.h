#pragma once

#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <IChannelLink.h>
#include <ISampleReceiver.h>
#include <ISampleBuffer.h>
#include <ISampleContainer.h>
#include <ISampleJoiner.h>
#include <ISampleSharer.h>

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
				static IChannelLink* CreateChannelLink();
				static IChannelLink* CreateChannelLink(ISampleContainer* pInput, ISampleReceiver* pOutput, float volume, float pan);

				static ISampleReceiver* CreateMeterChannel(int sampleRate);

				static ISampleBuffer* CreateSampleBuffer(int sampleCount);

				static ISampleJoiner* CreateSampleJoiner(int sampleCount);

				static ISampleSharer* CreateSampleSharer();

			private:
				ObjectFactory();
			};
		}
	}
}
