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

				void OnNextBuffer(bool writeSecondHalf);

				int AddProcessor(ISampleProcessorPtr& processor);

				int InsertProcessorBefore(ISampleProcessorPtr& processor, int processorId);

				bool RemoveProcessor(int processorId);

				ISampleProcessorPtr get_Processor(int processorId);

				int get_InputChannelCount();

				IInputChannelPtr get_InputChannel(int inputChannelId);

				int get_OutputChannelPairCount();

				IOutputChannelPairPtr get_OutputChannelPair(int outputChannelId);

				ITransportPtr& get_Transport();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				int GetNextProcessorId();
				std::vector<std::pair<int, ISampleProcessorPtr>>::iterator GetProcessorsById(int processorId);
				std::mutex m_processing_mutex;

				std::vector<std::pair<int, ISampleProcessorPtr>> m_processors;
				ISampleContainerPtr m_container;
				ITransportPtr m_transport;
				std::vector<IInputChannelPtr> m_inputChannels;
				std::vector<IOutputChannelPairPtr> m_outputChannelPairs;
				int m_currentMonitorInputId;
			};
		}
	}
}
