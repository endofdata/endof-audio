#include "pch.h"
#include "LooperConfig.h"


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Linq;
using namespace Audio::Asio::Interop;


LooperConfig::LooperConfig()
{
	_inputChannelList = gcnew List<int>();
	_outputChannelList = gcnew List<int>();
}

IEnumerable<int>^ LooperConfig::InputChannelList::get()
{
	return _inputChannelList;
}

IEnumerable<int>^ LooperConfig::OutputChannelList::get()
{
	return _outputChannelList;
}

void LooperConfig::AddInputChannelList(IEnumerable<int>^ selectedChannels)
{
	if (selectedChannels == nullptr)
	{
		throw gcnew ArgumentNullException("selectedChannels");
	}
	_inputChannelList->AddRange(Enumerable::Except(selectedChannels, _inputChannelList));
}

void LooperConfig::AddOutputChannelList(IEnumerable<int>^ selectedChannels)
{
	if (selectedChannels == nullptr)
	{
		throw gcnew ArgumentNullException("selectedChannels");
	}
	_outputChannelList->AddRange(Enumerable::Except(selectedChannels, _outputChannelList));
}
