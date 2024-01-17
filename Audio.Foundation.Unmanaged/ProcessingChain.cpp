#include "pch.h"
#include "ProcessingChain.h"
#include "ProcessingContext.h"
#include <functional>
#include <algorithm>

using namespace Audio::Foundation::Unmanaged;

ProcessingChain::ProcessingChain(ITransportPtr& transport, ISampleContainerPtr& sampleContainer) :
	m_transport(transport),
	m_container(sampleContainer),
	m_currentMonitorInputId(-1),
	m_isActive(false),
	m_refCount(0)
{
}

ProcessingChain::~ProcessingChain()
{
}


IMPLEMENT_IUNKNOWN(ProcessingChain)


void* ProcessingChain::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == __uuidof(IProcessingChain))
	{
		return dynamic_cast<IProcessingChain*>(this);
	}
	return nullptr;
}

void ProcessingChain::OnNextBuffer(bool writeSecondHalf)
{
	if (m_isActive)
	{
		const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

		bool readSecondHalf = !writeSecondHalf;

		m_container->Clear();

		ProcessingContext context = m_transport->Pulse();
		IHostClockPtr clock = m_transport->HostClock;

		std::for_each(m_inputChannels.begin(), m_inputChannels.end(), [this, readSecondHalf]
		(IInputChannelPtr& input)
		{
			input->OnNextBuffer(m_container, readSecondHalf);
		});

		std::for_each(m_processors.begin(), m_processors.end(), [this, context]
		(std::pair<int, ISampleProcessorPtr>& item)
		{
			item.second->Process(m_container, context);
		});

		// Fade at loop end / start with 10ms or at least one buffer
		int fadeWidth = std::min(m_container->SampleCount, (int)(clock->SampleRate / 100));
		FadeBuffers(context.IsLoopStart? fadeWidth: 0, context.IsLoopEnd? fadeWidth : 0);

		int firstOut = 0;

		std::for_each(m_outputChannelPairs.begin(), m_outputChannelPairs.end(), [this, writeSecondHalf, &firstOut]
		(IOutputChannelPairPtr& output)
		{
			output->OnNextBuffer(m_container, writeSecondHalf, firstOut);
			firstOut += 2;
		});

		if (m_mixRecorder != nullptr)
		{
			m_mixRecorder->Process(m_container, context);
		}
	}
}

void ProcessingChain::FadeBuffers(int fadeIn, int fadeOut)
{
	int sampleCount = m_container->SampleCount;
	int channelCount = m_container->ChannelCount;

	fadeIn = std::min(sampleCount, fadeIn);
	fadeOut = std::min(sampleCount, fadeOut);

	if (fadeIn > 0 || fadeOut > 0)
	{
		double fadeInFac = fadeIn;
		double fadeOutFac = fadeOut;

		std::function<Sample(Sample, int)> fadeInFunc = [fadeInFac](Sample sample, int index) { return sample * static_cast<Sample>((double)index / fadeInFac); };
		std::function<Sample(Sample, int)> fadeOutFunc = [fadeOutFac](Sample sample, int index) { return sample * static_cast<Sample>((fadeOutFac - 1.0 - (double)index) / fadeOutFac); };

		for (int c = 0; c < channelCount; c++)
		{
			Sample* target = m_container->Channels[c]->SamplePtr;

			for (int s = 0; s < fadeIn; s++)
			{
				*target++ = fadeInFunc(*target, s);
			}

			target += sampleCount - fadeIn - fadeOut;

			for (int s = 0; s < fadeOut; s++)
			{
				*target++ = fadeOutFunc(*target, s);
			}
		}
	}
}

void ProcessingChain::InitShutDown()
{
	const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);
	m_isActive = false;
	RemoveAllProcessors();
	RemoveAllInputChannels();
	RemoveAllOutputChannels();
}

void ProcessingChain::AddInput(IInputChannelPtr& input)
{
	if (input.GetInterfacePtr() != nullptr)
	{
		const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

		m_inputChannels.push_back(input);
	}
}

void ProcessingChain::AddOutputPair(IOutputChannelPairPtr& output)
{
	if (output.GetInterfacePtr() != nullptr)
	{
		const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

		m_outputChannelPairs.push_back(output);
	}
}


int ProcessingChain::AddProcessor(ISampleProcessorPtr& processor)
{
	if (processor.GetInterfacePtr() == nullptr)
	{
		throw std::invalid_argument("Sample processor pointer cannot be null.");
	}

	const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

	int nextId = GetNextProcessorId();
	m_processors.push_back(std::pair<int, ISampleProcessorPtr>(nextId, processor));

	return nextId;
}

int ProcessingChain::InsertProcessorBefore(ISampleProcessorPtr& processor, int processorId)
{
	const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

	int nextId = GetNextProcessorId();
	m_processors.insert(GetProcessorsById(processorId), std::pair<int, ISampleProcessorPtr>(nextId, processor));

	return nextId;
}

bool ProcessingChain::RemoveProcessor(int processorId)
{
	const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

	auto iter = GetProcessorsById(processorId);

	if (iter != m_processors.end())
	{
		m_processors.erase(iter);
		return true;
	}
	return false;
}

void ProcessingChain::RemoveAllProcessors()
{
	m_processors.clear();
}

ISampleProcessorPtr ProcessingChain::get_Processor(int processorId)
{
	auto iter = GetProcessorsById(processorId);
	if (iter == m_processors.end())
	{
		throw std::invalid_argument("Invalid processorId.");
	}
	return iter->second;
}

int ProcessingChain::GetNextProcessorId()
{
	auto maxElement = std::max_element(m_processors.begin(), m_processors.end(),
		[](std::pair<int, ISampleProcessorPtr>& a, std::pair<int, ISampleProcessorPtr>& b) { return a.first < b.first; });

	return (maxElement == m_processors.end()) ? 1 : maxElement->first + 1;
}

std::vector<std::pair<int, ISampleProcessorPtr>>::iterator ProcessingChain::GetProcessorsById(int processorId)
{
	return std::find_if(m_processors.begin(), m_processors.end(), [processorId](const std::pair<int, ISampleProcessorPtr>& item) { return item.first == processorId; });
}

IInputChannelPtr ProcessingChain::FindInput(int inputId)
{
	auto match = std::find_if(m_inputChannels.begin(), m_inputChannels.end(), [inputId](IInputChannelPtr& input) { return input->Id == inputId; });

	if (match == m_inputChannels.end())
	{
		return nullptr;
	}
	return *match;
}

IOutputChannelPairPtr ProcessingChain::FindOutput(int outputId)
{
	auto match = std::find_if(m_outputChannelPairs.begin(), m_outputChannelPairs.end(), [outputId](IOutputChannelPairPtr& output) { return output->Id == outputId; });

	if (match == m_outputChannelPairs.end())
	{
		return nullptr;
	}
	return *match;
}


void ProcessingChain::RemoveAllInputChannels()
{
	const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

	m_inputChannels.clear();
}

void ProcessingChain::RemoveAllOutputChannels()
{
	const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

	m_outputChannelPairs.clear();
}

void ProcessingChain::SetInputMonitoring(int inputChannelId, int outputChannelPairId)
{
	const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

	if (m_currentMonitorInputId >= 0)
	{
		auto currentInput = FindInput(m_currentMonitorInputId);
		if (currentInput != nullptr)
		{
			IOutputChannelPairPtr empty = nullptr;
			currentInput->DirectMonitor = empty;
		}
		m_currentMonitorInputId = -1;
	}

	auto newInput = FindInput(inputChannelId);

	if(newInput != nullptr)
	{
		auto newOutput = FindOutput(outputChannelPairId);
		if(newOutput != nullptr)
		{
			newInput->DirectMonitor = newOutput;
		}
	}
}

bool ProcessingChain::get_IsActive()
{
	return m_isActive;
}

void ProcessingChain::put_IsActive(bool value)
{
	m_isActive = value;
}

int ProcessingChain::get_InputChannelCount()
{
	return (int)m_inputChannels.size();
}

IInputChannelPtr ProcessingChain::get_InputChannel(int index)
{
	if (index < 0 || index >= InputChannelCount)
	{
		return nullptr;
	}
	return m_inputChannels[index];
}

int ProcessingChain::get_OutputChannelPairCount()
{
	return (int)m_outputChannelPairs.size();
}

IOutputChannelPairPtr ProcessingChain::get_OutputChannelPair(int index)
{
	if (index < 0 || index >= OutputChannelPairCount)
	{
		return nullptr;
	}
	return m_outputChannelPairs[index];
}

ITransportPtr& ProcessingChain::get_Transport()
{
	return m_transport;
}

ISampleProcessorPtr& ProcessingChain::get_MixRecorder()
{
	return m_mixRecorder;
}

void ProcessingChain::put_MixRecorder(ISampleProcessorPtr& value)
{
	const std::lock_guard<std::recursive_mutex> lock(m_processing_mutex);

	m_mixRecorder = value;
}

