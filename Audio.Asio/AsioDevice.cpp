#include "pch.h"
#include "AsioDevice.h"
#include "AsioCore.h"
#include "DriverRegistry.h"

using namespace Audio::Asio;
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
	m_activeInputChannels = gcnew List<int>();
	m_activeOutputChannels = gcnew List<int>();
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
	DetachBufferSwitchHandler();
	delete m_pCore;
}

void AsioDevice::ShowControlPanel()
{
	m_pCore->ShowControlPanel();
}

IInputChannel* AsioDevice::GetInputChannel(int index)
{
	return m_pCore->InputChannel[index];	
}

IOutputChannelPair* AsioDevice::GetOutputChannelPair(int index)
{
	return m_pCore->OutputChannelPair[index];
}

void AsioDevice::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}

void AsioDevice::AttachBufferSwitchHandler(BufferSwitchManagedCallback^ bufferSwitchHandler)
{
	if (bufferSwitchHandler == nullptr)
	{
		throw gcnew ArgumentNullException("Parameter 'bufferSwitchHandler' cannot be null.");
	}

	if (m_delegateHandle.IsAllocated)
	{
		DetachBufferSwitchHandler();
	}
	//BufferSwitchManagedCallback^ delegateInstance = gcnew BufferSwitchManagedCallback(this, &AsioDevice::test);
	m_delegateHandle = GCHandle::Alloc(bufferSwitchHandler);
	IntPtr functionPointer = Marshal::GetFunctionPointerForDelegate(bufferSwitchHandler);
	m_pCore->BufferSwitch = static_cast<BufferSwitchEventHandler>(functionPointer.ToPointer());
}

void AsioDevice::DetachBufferSwitchHandler()
{
	IsPoweredOn = false;
	m_pCore->BufferSwitch = NULL;

	if (m_delegateHandle.IsAllocated)
	{
		m_delegateHandle.Free();
	}
}


String^ AsioDevice::DriverName::get()
{
	char acTemp[256];
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
	return m_pCore->InputChannelCount;
}

int AsioDevice::OutputChannelPairCount::get()
{
	return m_pCore->OutputChannelPairCount;
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

	int channelId = 0;
	for each (String ^ name in m_pCore->GetKnownInputChannels())
	{
		result->Add(channelId++, name);
	}
	return result;
}

System::Collections::Generic::IDictionary<int, String^>^ AsioDevice::AvailableOutputChannels::get()
{
	Dictionary<int, String^>^ result = gcnew Dictionary<int, String^>;

	int channelId = 0;
	for each (String ^ name in m_pCore->GetKnownOutputChannels())
	{
		result->Add(channelId++, name);
	}
	return result;
}

bool AsioDevice::SelectInputChannel(int channelId, bool isSelected)
{
	if (isSelected)
	{
		if (!m_activeInputChannels->Contains(channelId))
		{
			m_activeInputChannels->Add(channelId);
		}
		return true;
	}
	return m_activeInputChannels->Remove(channelId);
}

bool AsioDevice::SelectOutputChannel(int channelId, bool isSelected)
{
	if (isSelected)
	{
		if (!m_activeOutputChannels->Contains(channelId))
		{
			m_activeOutputChannels->Add(channelId);
		}
		return true;
	}
	return m_activeOutputChannels->Remove(channelId);
}

void AsioDevice::ActivateChannels()
{
	m_pCore->CreateBuffers(m_activeInputChannels->ToArray(), m_activeOutputChannels->ToArray(), AsioCore::UsePreferredSize);
}
