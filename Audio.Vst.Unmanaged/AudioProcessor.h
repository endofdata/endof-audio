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
			class AudioProcessor : public ISampleProcessor
			{
			public:
				AudioProcessor(IAudioProcessorPtr& processor, IParameterChangesPtr& parameterChanges, int sampleCount, int sampleRate);
				virtual ~AudioProcessor();

				bool get_IsActive() const;
				void put_IsActive(bool value);
				_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);

				void Initialize(int sampleCount, int sampleRate);

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				AudioProcessor& operator =(const AudioProcessor& other);

				DECLARE_IUNKNOWN

			private:
				IAudioProcessorPtr m_processor;
				IComponentPtr m_component;
				IParameterChangesPtr m_parameterChanges;
				ProcessData m_processData;
				AudioBusBuffers m_inputBusBuffers;
				AudioBusBuffers m_outputBusBuffers;
				Sample* m_pSamplePtr[2];

				void InitProcessData(int sampleCount, int sampleRate);
				bool ConfigureBusArrangements();
				void UpdateProcessData(ISampleContainerPtr& container);

				bool m_canProcess64Bit;
				bool m_isBypassed;
				bool m_isActive;
			};
			
		}
	}
}