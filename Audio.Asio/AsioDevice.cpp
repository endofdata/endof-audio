#include "pch.h"
#include "AsioDevice.h"
#include <AsioCore.h>
#include "DriverRegistry.h"

using namespace Audio::Asio;
using namespace Audio::Asio::Unmanaged;
using namespace Audio::Asio::Interop;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Runtime::InteropServices;
using namespace Audio::Foundation::Abstractions;
using namespace System::Text::RegularExpressions;
using namespace System::Collections::Generic;

// static 
AsioDevice^ AsioDevice::CreateFromGuid(Guid guid)
{
	AsioCore* pCore = AsioCore::CreateInstance(reinterpret_cast<GUID&>(guid));

	if (pCore == nullptr)
	{
		throw gcnew InvalidOperationException("AsioCore::InitInstance() returned null value.");
	}

	return gcnew AsioDevice(pCore);
}

// static 
AsioDevice^ AsioDevice::CreateFromName(String^ name)
{
	if (name == nullptr)
	{
		throw gcnew ArgumentNullException("name");
	}

	DriverRegistry^ driverRegistry = DriverRegistry::FromRegistry();

	for each (RegisteredDriver ^ driver in driverRegistry)
	{
		if (String::Equals(driver->Name, name, StringComparison::OrdinalIgnoreCase))
		{
			return CreateFromGuid(driver->ClsId);
		}
	}
	throw gcnew ArgumentException(String::Format("No driver named '{0}' was found.", name));
}

// static 
AsioDevice^ AsioDevice::CreateFromNameLike(String^ pattern)
{
	if (pattern == nullptr)
	{
		throw gcnew ArgumentNullException("pattern");
	}

	DriverRegistry^ driverRegistry = DriverRegistry::FromRegistry();
	Regex^ regex = gcnew Regex(pattern);

	for each (RegisteredDriver^ driver in driverRegistry)
	{
		if (regex->IsMatch(driver->Name))
		{
			return CreateFromGuid(driver->ClsId);
		}
	}
	throw gcnew ArgumentException(String::Format("No driver with a name matching '{0}' was found.", pattern));
}

AsioDevice::AsioDevice(AsioCore* pCore)
{
	if (pCore == nullptr)
	{
		throw gcnew ArgumentNullException("Parameter 'pCore' cannot be null.");
	}
	m_pCore = pCore;
	m_activeInputChannelIds = gcnew List<int>();
	m_activeOutputChannelIds = gcnew List<int>();
}

AsioDevice::~AsioDevice()
{
	CleanUp(true);
}

AsioDevice::!AsioDevice()
{
	CleanUp(false);
}

void AsioDevice::CleanUp(bool fIsDisposing)
{
	delete m_pCore;
}

void AsioDevice::ShowControlPanel()
{
	m_pCore->ShowControlPanel();
}

IInputChannelPtr AsioDevice::GetInputChannel(int channelId)
{
	return m_pCore->ProcessingChain->InputChannel[channelId];	
}

IOutputChannelPairPtr AsioDevice::GetOutputChannelPair(int channelPairId)
{
	return m_pCore->ProcessingChain->OutputChannelPair[channelPairId];
}

void AsioDevice::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

String^ AsioDevice::DriverName::get()
{
	char acTemp[MaxAsioDriverName];
	ZeroMemory(acTemp, sizeof(acTemp));
	m_pCore->GetDriverName(acTemp);
	return gcnew String(acTemp);
}

int AsioDevice::DriverVersion::get()
{
	return m_pCore->GetDriverVersion();
}

int AsioDevice::InputChannelCount::get()
{
	return m_pCore->ProcessingChain->InputChannelCount;
}

int AsioDevice::OutputChannelPairCount::get()
{
	return m_pCore->ProcessingChain->OutputChannelPairCount;
}

int AsioDevice::SampleCount::get()
{
	return m_pCore->SampleCount;
}

int AsioDevice::SampleRate::get()
{
	return (int)m_pCore->SampleRate;
}

void AsioDevice::SampleRate::set(int sampleRate)
{
	m_pCore->SampleRate = (double)sampleRate;
}

Boolean AsioDevice::IsPoweredOn::get()
{
	return m_isPoweredOn;
}

void AsioDevice::IsPoweredOn::set(Boolean value)
{
	if (value != m_isPoweredOn)
	{
		m_isPoweredOn = value;

		if (value == true)
		{
			m_pCore->Start();
		}
		else
		{
			m_pCore->Stop();
		}

		//TraceSource->TraceEvent(TraceEventType::Information, 8, "IsPoweredOn = {0}", value);

		OnPropertyChanged(IsPoweredOnProperty);
	}
}

System::Collections::Generic::IDictionary<int, String^>^ AsioDevice::AvailableInputChannels::get()
{
	Dictionary<int, String^>^ result = gcnew Dictionary<int, String^>;

	int channelMax = m_pCore->HardwareInputCount;
	char buffer[42];

	for(int channelId = 0; channelId < channelMax; channelId++)
	{
		int len = m_pCore->GetHardwareInputName(channelId, buffer, _countof(buffer));
		String^ name = gcnew String(buffer);
		result->Add(channelId++, name);
	}
	return result;
}

System::Collections::Generic::IDictionary<int, String^>^ AsioDevice::AvailableOutputChannels::get()
{
	Dictionary<int, String^>^ result = gcnew Dictionary<int, String^>;

	int channelMax = m_pCore->HardwareOutputCount;
	char buffer[Audio::Asio::Unmanaged::MaxAsioChannelName];

	for(int channelId = 0; channelId < channelMax; channelId++)
	{
		int len = m_pCore->GetHardwareOutputName(channelId, buffer, _countof(buffer));
		String^ name = gcnew String(buffer);
		result->Add(channelId++, name);
	}
	return result;
}

bool AsioDevice::SelectInputChannel(int channelId, bool isSelected)
{
	if (isSelected)
	{
		if (!m_activeInputChannelIds->Contains(channelId))
		{
			m_activeInputChannelIds->Add(channelId);
		}
		return true;
	}
	return m_activeInputChannelIds->Remove(channelId);
}

bool AsioDevice::SelectOutputChannel(int channelId, bool isSelected)
{
	if (isSelected)
	{
		if (!m_activeOutputChannelIds->Contains(channelId))
		{
			m_activeOutputChannelIds->Add(channelId);
		}
		return true;
	}
	return m_activeOutputChannelIds->Remove(channelId);
}

void AsioDevice::ActivateChannels()
{
	array<int>^ inputChannelIds = m_activeInputChannelIds->ToArray();
	array<int>^ outputChannelIds = m_activeOutputChannelIds->ToArray();

	pin_ptr<int> pInputIds = inputChannelIds->Length > 0? &inputChannelIds[0] : nullptr;
	pin_ptr<int> pOutputIds = outputChannelIds->Length > 0? &outputChannelIds[0] : nullptr;

	m_pCore->CreateBuffers(pInputIds, inputChannelIds->Length, pOutputIds, outputChannelIds->Length, AsioCore::UsePreferredSize);
}
