#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "IProcessingChain.h"
#include "IInputChannel.h"
#include <vector>

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
				ProcessingChain(ISampleContainerPtr& sampleContainer);

				virtual ~ProcessingChain();

				virtual void AddInput(IInputChannelPtr& input);

				virtual void AddOutputPair(IOutputChannelPairPtr& output);

				virtual void SetInputMonitoring(int inputId, int outputId);

				virtual IInputChannelPtr FindInput(int inputId);

				virtual IOutputChannelPairPtr FindOutput(int outputId);

				virtual void RemoveAllInputChannels();

				virtual void RemoveAllOutputChannels();

				virtual void OnNextBuffer(bool writeSecondHalf);

				virtual int AddProcessor(ISampleProcessorPtr& processor);

				virtual int InsertProcessorBefore(ISampleProcessorPtr& processor, int processorId);

				virtual bool RemoveProcessor(int processorId);

				ISampleProcessorPtr get_Processor(int processorId);

				int get_InputChannelCount();

				IInputChannelPtr get_InputChannel(int inputChannelId);

				int get_OutputChannelPairCount();

				IOutputChannelPairPtr get_OutputChannelPair(int outputChannelId);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				int GetNextProcessorId();
				std::vector<std::pair<int, ISampleProcessorPtr>>::iterator GetProcessorsById(int processorId);

				std::vector<std::pair<int, ISampleProcessorPtr>> m_processors;
				ISampleContainerPtr m_container;
				std::vector<IInputChannelPtr> m_inputChannels;
				std::vector<IOutputChannelPairPtr> m_outputChannelPairs;
				int m_currentMonitorInputId;
			};
		}
	}
}
