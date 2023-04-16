#pragma once
#include <Audio.Vst.Unmanaged.h>
#include <VstCom.h>
#include <ISampleProcessor.h>
#include <UnknownBase.h>

using namespace Steinberg::Vst;
using namespace Steinberg;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			class _AUDIO_VST_UNMANAGED_API AudioProcessor : public ISampleProcessor
			{
			public:
				AudioProcessor(IAudioProcessorPtr& processor, IParameterChangesPtr& parameterChanges, int sampleCount, int sampleRate);
				virtual ~AudioProcessor();

				bool get_IsActive() const;
				void put_IsActive(bool value);
				_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

				bool get_IsProcessing() const;
				void put_IsProcessing(bool value);
				_declspec(property(get = get_IsProcessing, put = put_IsProcessing)) bool IsProcessing;

				bool get_IsBypassed();
				void put_IsBypassed(bool value);

				void Initialize(int sampleCount, int sampleRate);

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				AudioProcessor& operator =(const AudioProcessor& other);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				IAudioProcessorPtr m_processor;
				IComponentPtr m_component;
				IParameterChangesPtr m_parameterChanges;
				ProcessData m_processData;
				AudioBusBuffers m_inputBusBuffers;
				AudioBusBuffers m_outputBusBuffers;
				Sample* m_pSamplePtr[2];

				void InitProcessData(int sampleCount, int sampleRate);
				void UpdateProcessData(ISampleContainerPtr& container);

				bool m_isBypassed;
				bool m_isActive;
			};
			
		}
	}
}