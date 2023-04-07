#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <ISampleProcessor.h>
#include <ISampleSource.h>
#include <ISampleBuffer.h>
#include <ISampleContainer.h>
#include <ISampleJoiner.h>
#include <ISampleSharer.h>
#include <IMeterChannel.h>
#include <ITake.h>
#include <ITakeSequence.h>
#include <IHostClock.h>
#include <IInputChannel.h>
#include <IProcessingChain.h>
#include <IMidiInput.h>
#include <IMidiEvents.h>
#include <IRecorder.h>
#include <string>

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

				static ISampleJoinerPtr CreateSampleJoiner();

				static IMeterChannelPtr CreateMeterChannel(int sampleRate, int channelCount);

				static ISampleProcessorPtr CreateToFileProcessor(const std::string& filename);

				static ISampleProcessorPtr CreateToStreamProcessor(std::ostream& output);

				static ISampleProcessorPtr CreateToContainerProcessor(ISampleContainerPtr& target);

				static IRecorderPtr CreateRecorder(int channelCount, int initialSize, int growth);

				static ISampleSourcePtr CreateFileSource(const std::string& filename);

				static ISampleSourcePtr CreateContainerSource(ISampleContainerPtr& source);

				static ISampleProcessorPtr CreateFromSourceProcessor(ISampleSourcePtr& source);

				static ITakePtr CreateTake(ISampleContainerPtr& container, AudioTime position, IHostClockPtr& hostClock);

				static ITakePtr CreateTake(ISampleContainerPtr& container, AudioTime position, AudioTime length);

				static IHostClockPtr CreateHostClock(int sampleRate = 0);

				static ITakeSequencePtr CreateTakeSequence(IHostClockPtr& hostClock);

				static IInputChannelPtr CreateInputChannel(int sampleType, int hwChannelId, void* pHwBufferA, void* pHwBufferB, int sampleCount);

				static IOutputChannelPairPtr CreateOutputChannelPair(int sampleTpe,
					int hwChannelId1, void* pBufferA1, void* pBufferB1, int hwChannelId2, void* pBufferA2, void* pBufferB2, int sampleCount);

				static IProcessingChainPtr CreateProcessingChain(ISampleContainerPtr& container);

				static IMidiInputPtr CreateMidiInput();

				static int SelectMidiInputDevice(MidiInCapsHandler handler);

			private:
				ObjectFactory();

				static int NextTakeId();

				static int LastTakeId;
			};
		}
	}
}
