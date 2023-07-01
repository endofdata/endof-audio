#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <ISampleProcessor.h>
#include <ISampleSource.h>
#include <ISampleBuffer.h>
#include <ISampleContainer.h>
#include <ISourceJoiner.h>
#include <ISampleSharer.h>
#include <IMeterChannel.h>
#include <ITake.h>
#include <ITakeSequence.h>
#include <IHostClock.h>
#include <IInputChannel.h>
#include <IProcessingChain.h>
#include <IMidiInput.h>
#include <IMidiEvents.h>
#include <IOscillator.h>
#include <IRecorder.h>
#include <ITransport.h>
#include <IController.h>
#include <ISpatial.h>
#include <string>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class _AUDIO_FOUNDATION_UNMANAGED_API FoundationObjectFactory
			{
			public:
				static ISampleBufferPtr CreateSampleBuffer(int sampleCount);

				static ISampleContainerPtr CreateSampleContainer(int sampleCount, int channelCount);

				static ISampleSharerPtr CreateSampleSharer();

				static ISourceJoinerPtr CreateSourceJoiner();

				static IMeterChannelPtr CreateMeterChannel(int sampleRate, int channelCount);

				static ISampleProcessorPtr CreateToFileProcessor(const std::wstring& filename);

				static ISampleProcessorPtr CreateToStreamProcessor(std::ostream& output);

				static ISampleProcessorPtr CreateToContainerProcessor(ISampleContainerPtr& target);

				static ISpatialPtr CreateSpatial(double level = 1.0f, double pan = 0.5f);

				static IRecorderPtr CreateRecorder(int channelCount, int initialSize, int growth);

				static ISampleSourcePtr CreateRawFileSource(const std::wstring& filename, int channelCount);

				static ISampleSourcePtr CreateContainerSource(ISampleContainerPtr& source);

				static ISampleProcessorPtr CreateFromSourceProcessor(ISampleSourcePtr& source);

				static ITakePtr CreateTake(ISampleContainerPtr& container, AudioTime position, IHostClockPtr& hostClock);

				static ITakePtr CreateTake(ISampleContainerPtr& container, AudioTime position, AudioTime length);

				static IHostClockPtr CreateHostClock(double sampleRate = 0.0);

				static ITransportPtr CreateTransport(IHostClockPtr& hostClock, int sampleCount);

				static IControllerPtr CreateMidiTransportControl(ITransportPtr& transport, int midiDevId);

				static ITakeSequencePtr CreateTakeSequence(ITransportPtr& transport);

				static IInputChannelPtr CreateInputChannel(int sampleType, int hwChannelId, void* pHwBufferA, void* pHwBufferB, int sampleCount);

				static IOutputChannelPairPtr CreateOutputChannelPair(int sampleTpe,
					int hwChannelId1, void* pBufferA1, void* pBufferB1, int hwChannelId2, void* pBufferA2, void* pBufferB2, int sampleCount, float saturation = 0.0f);

				static IProcessingChainPtr CreateProcessingChain(ITransportPtr& transport, ISampleContainerPtr& container);

				static IOscillatorPtr CreateTestOscillator(double sampleRate, double frequency, double amplitude);

				static IMidiInputPtr CreateMidiInput();

				static int SelectMidiInputDevice(MidiInCapsHandler handler, void* callbackParam = nullptr);

			private:
				FoundationObjectFactory();

				static int NextTakeId();

				static int LastTakeId;
			};
		}
	}
}
