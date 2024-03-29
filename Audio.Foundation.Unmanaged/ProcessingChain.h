#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "IProcessingChain.h"
#include "IInputChannel.h"
#include "ITransport.h"
#include <vector>
#include <mutex>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class ProcessingChain : public IProcessingChain
			{
			public:
				ProcessingChain(ITransportPtr& transport, ISampleContainerPtr& sampleContainer);

				virtual ~ProcessingChain();

				void AddInput(IInputChannelPtr& input);

				void AddOutputPair(IOutputChannelPairPtr& output);

				void SetInputMonitoring(int inputId, int outputId);

				IInputChannelPtr FindInput(int inputId);

				IOutputChannelPairPtr FindOutput(int outputId);

				void RemoveAllInputChannels();

				void RemoveAllOutputChannels();

				void RemoveAllProcessors();

				void OnNextBuffer(bool writeSecondHalf);

				void InitShutDown();

				int AddProcessor(ISampleProcessorPtr& processor);

				int InsertProcessorBefore(ISampleProcessorPtr& processor, int processorId);

				bool RemoveProcessor(int processorId);

				bool get_IsActive();
				void put_IsActive(bool value);

				ISampleProcessorPtr get_Processor(int processorId);

				int get_InputChannelCount();

				IInputChannelPtr get_InputChannel(int inputChannelId);

				int get_OutputChannelPairCount();

				IOutputChannelPairPtr get_OutputChannelPair(int outputChannelId);

				ITransportPtr& get_Transport();

				ISampleProcessorPtr& get_MixRecorder();
				void put_MixRecorder(ISampleProcessorPtr& value);

				DECLARE_IUNKNOWN

			protected:
				void FadeBuffers(int fadeIn, int fadeOut);

			private:
				int GetNextProcessorId();
				std::vector<std::pair<int, ISampleProcessorPtr>>::iterator GetProcessorsById(int processorId);
				std::recursive_mutex m_processing_mutex;

				std::vector<std::pair<int, ISampleProcessorPtr>> m_processors;
				ISampleContainerPtr m_container;
				ITransportPtr m_transport;
				ISampleProcessorPtr m_mixRecorder;
				std::vector<IInputChannelPtr> m_inputChannels;
				std::vector<IOutputChannelPairPtr> m_outputChannelPairs;
				int m_currentMonitorInputId;
				bool m_isActive;
			};
		}
	}
}
