#include "pch.h"
#include "AsioRouter.h"
#include "AsioDevice.h"
#include "AudioOutput.h"
#include "AudioInput.h"

using namespace System;
using namespace Audio::Asio;
using namespace Audio::Asio::Interop;

AsioRouter::AsioRouter(AsioDevice^ outputDevice, AsioDevice^ inputDevice)
{
	if (outputDevice == nullptr)
	{
		throw gcnew ArgumentNullException("Parameter '{devices}' cannot be null.");
	}
	if (inputDevice != nullptr &&  outputDevice->SampleCount != inputDevice->SampleCount)
	{
		throw gcnew NotSupportedException("Output device and input device cannot use different buffer sizes.");
	}

	m_outputDevice = outputDevice;
	m_audioOutputPairs = gcnew AudioOutputCollection();

	m_inputDevice = inputDevice;
	m_audioInputs = gcnew AudioInputCollection();

	for (int o = 0; o < outputDevice->OutputChannelPairCount; o++)
	{
		IOutputChannelPair* pOutputPair = outputDevice->GetOutputChannelPair(o);
		if (pOutputPair == NULL)
		{
			throw gcnew InvalidOperationException("Output channel pair is not allocated.");
		}
		m_audioOutputPairs->Add(gcnew AudioOutput(outputDevice->SampleRate, outputDevice->SampleCount, pOutputPair, o + 1));
	}

	if (inputDevice != nullptr)
	{
		for (int i = 0; i < inputDevice->InputChannelCount; i++)
		{
			IInputChannel* pInput = inputDevice->GetInputChannel(i);
			if(pInput == NULL)
			{
				throw gcnew InvalidOperationException("Input channel is not allocated.");
			}
			m_audioInputs->Add(gcnew AudioInput(inputDevice->SampleRate, pInput, i + 1));
		}
	}

	// Do not attach buffer switch handler before initialization is complete: the device may already be running
	if (outputDevice != inputDevice)
	{
		if (inputDevice != nullptr)
		{
			inputDevice->AttachBufferSwitchHandler(gcnew BufferSwitchManagedCallback(this, &AsioRouter::OnInputBufferSwitch));
		}
		outputDevice->AttachBufferSwitchHandler(gcnew BufferSwitchManagedCallback(this, &AsioRouter::OnOutputBufferSwitch));
	}
	else
	{
		outputDevice->AttachBufferSwitchHandler(gcnew BufferSwitchManagedCallback(this, &AsioRouter::OnDuplexBufferSwitch));
	}

	//TraceSource->TraceEvent(TraceEventType::Information, 2, "Created {0} input channel(s) and {1} output channel pair(s).", m_audioInputs->Count, m_audioOutputPairs->Count);
}

AsioRouter::~AsioRouter()
{
	CleanUp(true);
}

AsioRouter::!AsioRouter()
{
	CleanUp(false);
}

void AsioRouter::CleanUp(bool isDisposing)
{
	if (m_inputDevice != nullptr && m_inputDevice != m_outputDevice)
	{
		delete m_inputDevice;
	}
	if (m_outputDevice != nullptr)
	{
		delete m_outputDevice;
	}
	if (m_audioOutputPairs != nullptr)
	{
		m_audioOutputPairs->Clear();
	}
	if (m_audioInputs != nullptr)
	{
		m_audioInputs->Clear();
	}
}

void AsioRouter::OnOutputBufferSwitch(bool writeSecondHalf)
{
	try
	{
		BufferSwitchManagedCallback^ handler = m_outputBufferSwitchHandler;

		if (handler != nullptr)
		{
			handler(writeSecondHalf);
		}

		for each (AudioOutput^ audioOutput in m_audioOutputPairs)
		{
			audioOutput->Send();
		}
	}
	catch (Exception^)
	{
		IsPoweredOn = false;

		// TraceSource->TraceEvent(TraceEventType::Critical, 9, ExceptionExtensions::GetMessageChain(ex));
	}
}

void AsioRouter::OnInputBufferSwitch(bool writeSecondHalf)
{
	try
	{
		BufferSwitchManagedCallback^ handler = m_inputBufferSwitchHandler;

		//for each (AudioInput ^ audioInput in m_audioInputs)
		//{
		//	audioInput->ReadCurrentFrame()
		//}
		if (handler != nullptr)
		{
			handler(!writeSecondHalf);
		}
	}
	catch (Exception^)
	{
		IsPoweredOn = false;

		// TraceSource->TraceEvent(TraceEventType::Critical, 9, ExceptionExtensions::GetMessageChain(ex));
	}
}

void AsioRouter::OnDuplexBufferSwitch(bool writeSecondHalf)
{
	OnInputBufferSwitch(writeSecondHalf);
	OnOutputBufferSwitch(writeSecondHalf);
}

void AsioRouter::AttachBufferSwitchHandler(BufferSwitchManagedCallback^ bufferSwitchHandler, bool isInput)
{
	if (bufferSwitchHandler == nullptr)
	{
		throw gcnew ArgumentNullException("Parameter 'bufferSwitchHandler' cannot be null.");
	}
	if (isInput)
	{
		m_inputBufferSwitchHandler = bufferSwitchHandler;
	}
	else
	{
		m_outputBufferSwitchHandler = bufferSwitchHandler;
	}
}

void AsioRouter::DetachBufferSwitchHandler(bool isInput)
{
	if (isInput)
	{
		m_inputBufferSwitchHandler = nullptr;
	}
	else
	{
		m_outputBufferSwitchHandler = nullptr;
	}
}

Boolean AsioRouter::IsPoweredOn::get()
{
	return m_isPoweredOn;
}

void AsioRouter::IsPoweredOn::set(Boolean value)
{
	if (value != m_isPoweredOn)
	{
		if (m_inputDevice != nullptr)
		{
			m_inputDevice->IsPoweredOn = value;
		}
		m_outputDevice->IsPoweredOn = value;
		m_isPoweredOn = value;

		OnPropertyChanged(IsPoweredOnProperty);
	}
}

AudioInputCollection^ AsioRouter::Inputs::get()
{
	return m_audioInputs;
}

AudioOutputCollection^ AsioRouter::Outputs::get()
{
	return m_audioOutputPairs;
}

AsioDevice^ AsioRouter::InputDevice::get()
{
	return m_inputDevice;
}

AsioDevice^ AsioRouter::OutputDevice::get()
{
	return m_outputDevice;
}

int AsioRouter::SampleCount::get()
{
	// c'tor checks that input and output are using the same buffer size
	// so we can always return output buffer size here.
	return m_outputDevice->SampleCount;
}

int AsioRouter::SampleRate::get()
{
	// c'tor checks that input and output are using the same sample rate
	// so we can always return output buffer size here.
	return m_outputDevice->SampleRate;
}

void AsioRouter::OnPropertyChanged(System::String^ propertyName)
{
	PropertyChanged(this, gcnew System::ComponentModel::PropertyChangedEventArgs(propertyName));
}