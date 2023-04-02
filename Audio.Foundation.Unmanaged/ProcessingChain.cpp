#include "pch.h"
#include "ProcessingChain.h"
#include <functional>
#include <algorithm>

using namespace Audio::Foundation::Unmanaged;

ProcessingChain::ProcessingChain(ISampleContainerPtr& sampleContainer) :
	m_container(sampleContainer),
	m_refCount(0)
{
}

ProcessingChain::~ProcessingChain()
{
}


IMPLEMENT_IUNKNOWN(ProcessingChain)


bool ProcessingChain::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(IProcessingChain))
	{
		*ppvResult = dynamic_cast<IProcessingChain*>(this);
		return true;
	}
	return false;

}

void ProcessingChain::OnNextBuffer(bool writeSecondHalf)
{
	int channel = 0;
	bool readSecondHalf = !writeSecondHalf;

	std::for_each(m_inputChannels.begin(), m_inputChannels.end(), [this, readSecondHalf, &channel]
	(IInputChannelPtr& input)
	{
		input->OnNextBuffer(m_container, readSecondHalf, channel++);
	});

	for (int unusedChannel = channel; unusedChannel < m_container->ChannelCount; unusedChannel++)
	{
		m_container->Channels[unusedChannel]->Clear();
	}

	std::for_each(m_processors.begin(), m_processors.end(), [this]
	(std::pair<int, ISampleProcessorPtr>& item)
	{
		item.second->Process(m_container);
	});

	int firstOut = 0;

	std::for_each(m_outputChannelPairs.begin(), m_outputChannelPairs.end(), [this, writeSecondHalf, &firstOut]
	(IOutputChannelPairPtr& output)
	{
		output->OnNextBuffer(m_container, writeSecondHalf, firstOut);
		firstOut += 2;
	});
}

void ProcessingChain::AddInput(IInputChannelPtr& input)
{
	m_inputChannels.push_back(input);
}

void ProcessingChain::AddOutputPair(IOutputChannelPairPtr& output)
{
	m_outputChannelPairs.push_back(output);
}


int ProcessingChain::AddProcessor(ISampleProcessorPtr& processor)
{
	int nextId = GetNextProcessorId();
	m_processors.push_back(std::pair<int, ISampleProcessorPtr>(nextId, processor));

	return nextId;
}

int ProcessingChain::InsertProcessorBefore(ISampleProcessorPtr& processor, int processorId)
{
	int nextId = GetNextProcessorId();
	m_processors.insert(GetProcessorsById(processorId), std::pair<int, ISampleProcessorPtr>(nextId, processor));

	return nextId;
}

bool ProcessingChain::RemoveProcessor(int processorId)
{
	auto iter = GetProcessorsById(processorId);

	if (iter != m_processors.end())
	{
		m_processors.erase(iter);
		return true;
	}
	return false;
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
	m_inputChannels.clear();
}

void ProcessingChain::RemoveAllOutputChannels()
{
	m_outputChannelPairs.clear();
}

void ProcessingChain::SetInputMonitoring(int inputChannelId, int outputChannelPairId)
{
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