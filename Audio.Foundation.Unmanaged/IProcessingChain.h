#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleProcessor.h>
#include <IInputChannel.h>
#include <IOutputChannelPair.h>
#include <ITransport.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("5c28ac3b-c3b6-47a0-948e-94d6462fcb9c")) IProcessingChain : public IUnknown
				{
					void OnNextBuffer(bool writeSecondHalf) = 0;

					void AddInput(IInputChannelPtr& input) = 0;

					void AddOutputPair(IOutputChannelPairPtr& output) = 0;

					void SetInputMonitoring(int inputId, int outputId) = 0;

					virtual IInputChannelPtr FindInput(int inputId) = 0;

					virtual IOutputChannelPairPtr FindOutput(int outputId) = 0;

					virtual void RemoveAllInputChannels() = 0;

					virtual void RemoveAllOutputChannels() = 0;


					int AddProcessor(ISampleProcessorPtr& processor) = 0;

					int InsertProcessorBefore(ISampleProcessorPtr& processor, int processorId) = 0;

					bool RemoveProcessor(int processorId) = 0;

					ISampleProcessorPtr get_Processor(int processorId) = 0;
					_declspec(property(get = get_Processor)) ISampleProcessorPtr Processor[];

					int get_InputChannelCount() = 0;
					_declspec(property(get = get_InputChannelCount)) int InputChannelCount;

					IInputChannelPtr get_InputChannel(int index) = 0;
					_declspec(property(get = get_InputChannel)) IInputChannelPtr InputChannel[];

					int get_OutputChannelPairCount() = 0;
					_declspec(property(get = get_OutputChannelPairCount)) int OutputChannelPairCount;

					IOutputChannelPairPtr get_OutputChannelPair(int index) = 0;
					_declspec(property(get = get_OutputChannelPair)) IOutputChannelPairPtr OutputChannelPair[];

					ITransportPtr& get_Transport() = 0;
					_declspec(property(get = get_Transport)) ITransportPtr Transport;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IProcessingChain, __uuidof(IProcessingChain));

			}
		}
	}
}
