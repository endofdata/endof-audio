#include "pch.h"
#include "AsioDevice.h"
#include "AsioCore.h"

using namespace Audio::Asio;
using namespace Audio::Asio::Interop;
using namespace System;
using namespace System::ComponentModel;
using namespace System::Runtime::InteropServices;
using namespace Audio::Foundation::Abstractions;

// static 
AsioDevice^ AsioDevice::CreateFromGuid(Guid guid)
{
	//TraceSource->TraceEvent(TraceEventType::Information, 0, "Initializing ASIO core for driver GUID {0}", guid);

	AsioCore* pCore = AsioCore::CreateInstance(reinterpret_cast<GUID&>(guid), AsioCore::UsePreferredSize);

	if (pCore == nullptr)
	{
		throw gcnew InvalidOperationException("AsioCore::InitInstance() returned null value.");
	}

	try
	{
		//TraceSource->TraceEvent(TraceEventType::Information, 10, "Using ASIO driver '{0}' version {1}.", DriverName, DriverVersion);
		return gcnew AsioDevice(pCore);
	}
	catch (Exception^)
	{
		delete pCore;
		throw;
	}
}

AsioDevice::AsioDevice(AsioCore* pCore)
{
	if (pCore == nullptr)
	{
		throw gcnew ArgumentNullException("Parameter 'pCore' cannot be null.");
	}
	m_pCore = pCore;
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
