#include "pch.h"
#include "ManagedLooperConfig.h"


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Linq;
using namespace Audio::Asio::Interop;


ManagedLooperConfig::ManagedLooperConfig()
{
	_inputChannelList = gcnew List<int>();
	_outputChannelList = gcnew List<int>();
}

IEnumerable<int>^ ManagedLooperConfig::InputChannelList::get()
{
	return _inputChannelList;
}

IEnumerable<int>^ ManagedLooperConfig::OutputChannelList::get()
{
	return _outputChannelList;
}

void ManagedLooperConfig::AddInputChannelList(IEnumerable<int>^ selectedChannels)
{
	if (selectedChannels == nullptr)
	{
		throw gcnew ArgumentNullException("selectedChannels");
	}
	_inputChannelList->AddRange(Enumerable::Except(selectedChannels, _inputChannelList));
}

void ManagedLooperConfig::AddOutputChannelList(IEnumerable<int>^ selectedChannels)
{
	if (selectedChannels == nullptr)
	{
		throw gcnew ArgumentNullException("selectedChannels");
	}
	_outputChannelList->AddRange(Enumerable::Except(selectedChannels, _outputChannelList));
}
