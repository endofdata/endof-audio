#include "pch.h"
#include "AsioCoreCallbacks.h"
#include "AsioCore.h"
#include "AsioCoreException.h"

using namespace System;
using namespace Audio::Asio;

// static
AsioCoreCallbacks* AsioCoreCallbacks::s_UsedDevices[] = { NULL, NULL, NULL };
// static, must match number of items in s_UsedDevices which is limited by number of EntryPoints
const int AsioCoreCallbacks::MaxDeviceCount = 3;

// static
AsioCoreCallbacks& AsioCoreCallbacks::Create(AsioCore* pCore)
{
	for (int index = 0; index < MaxDeviceCount; index++)
	{
		if (s_UsedDevices[index] == NULL)
		{
			ASIOCallbacks callbacks;

			switch (index)
			{
			case 0:
				callbacks.bufferSwitch = &BufferSwitchEntryDev0;
				callbacks.bufferSwitchTimeInfo = &BufferSwitchTimeInfoEntryDev0;
				callbacks.asioMessage = &AsioMessageEntryDev0;
				callbacks.sampleRateDidChange = &SampleRateDidChangeEntryDev0;
				break;
			case 1:
				callbacks.bufferSwitch = &BufferSwitchEntryDev1;
				callbacks.bufferSwitchTimeInfo = &BufferSwitchTimeInfoEntryDev1;
				callbacks.asioMessage = &AsioMessageEntryDev1;
				callbacks.sampleRateDidChange = &SampleRateDidChangeEntryDev1;
				break;
			case 2:
				callbacks.bufferSwitch = &BufferSwitchEntryDev2;
				callbacks.bufferSwitchTimeInfo = &BufferSwitchTimeInfoEntryDev2;
				callbacks.asioMessage = &AsioMessageEntryDev2;
				callbacks.sampleRateDidChange = &SampleRateDidChangeEntryDev2;
				break;
			}

			s_UsedDevices[index] = new AsioCoreCallbacks(pCore, callbacks, index);

			return *(s_UsedDevices[index]);
		}
	}
	throw gcnew InvalidOperationException("Maximum number of active devices exceeded.");
}

AsioCoreCallbacks::AsioCoreCallbacks(AsioCore* pCore, ASIOCallbacks& callbacks, int deviceIndex) : 
	m_pCore(pCore), m_callbacks(callbacks), m_deviceIndex(deviceIndex)
{
	if (pCore == NULL)
	{
		throw gcnew ArgumentNullException("Parameter 'pCore' cannot be null.");
	}
}

AsioCoreCallbacks::~AsioCoreCallbacks()
{
	s_UsedDevices[m_deviceIndex] = NULL;
}

ASIOCallbacks& AsioCoreCallbacks::get_Callbacks()
{
	return m_callbacks;
}


// static
void AsioCoreCallbacks::BufferSwitchEntryDev0(long doubleBufferIndex, ASIOBool directProcess)
{
	s_UsedDevices[0]->m_pCore->OnBufferSwitch(doubleBufferIndex, directProcess);
}

// static
ASIOTime* AsioCoreCallbacks::BufferSwitchTimeInfoEntryDev0(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	return s_UsedDevices[0]->m_pCore->OnBufferSwitchTimeInfo(params, doubleBufferIndex, directProcess);
}

// static
void AsioCoreCallbacks::SampleRateDidChangeEntryDev0(ASIOSampleRate rate)
{
	s_UsedDevices[0]->m_pCore->OnSampleRateDidChange(rate);
}

// static
long AsioCoreCallbacks::AsioMessageEntryDev0(long selector, long value, void* message, double* opt)
{
	return s_UsedDevices[0]->m_pCore->OnAsioMessage(selector, value, message, opt);
}


// static
void AsioCoreCallbacks::BufferSwitchEntryDev1(long doubleBufferIndex, ASIOBool directProcess)
{
	s_UsedDevices[1]->m_pCore->OnBufferSwitch(doubleBufferIndex, directProcess);
}

// static
ASIOTime* AsioCoreCallbacks::BufferSwitchTimeInfoEntryDev1(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	return s_UsedDevices[1]->m_pCore->OnBufferSwitchTimeInfo(params, doubleBufferIndex, directProcess);
}

// static
void AsioCoreCallbacks::SampleRateDidChangeEntryDev1(ASIOSampleRate rate)
{
	s_UsedDevices[1]->m_pCore->OnSampleRateDidChange(rate);
}

// static
long AsioCoreCallbacks::AsioMessageEntryDev1(long selector, long value, void* message, double* opt)
{
	return s_UsedDevices[1]->m_pCore->OnAsioMessage(selector, value, message, opt);
}



// static
void AsioCoreCallbacks::BufferSwitchEntryDev2(long doubleBufferIndex, ASIOBool directProcess)
{
	s_UsedDevices[2]->m_pCore->OnBufferSwitch(doubleBufferIndex, directProcess);
}

// static
ASIOTime* AsioCoreCallbacks::BufferSwitchTimeInfoEntryDev2(ASIOTime* params, long doubleBufferIndex, ASIOBool directProcess)
{
	return s_UsedDevices[2]->m_pCore->OnBufferSwitchTimeInfo(params, doubleBufferIndex, directProcess);
}

// static
void AsioCoreCallbacks::SampleRateDidChangeEntryDev2(ASIOSampleRate rate)
{
	s_UsedDevices[2]->m_pCore->OnSampleRateDidChange(rate);
}

// static
long AsioCoreCallbacks::AsioMessageEntryDev2(long selector, long value, void* message, double* opt)
{
	return s_UsedDevices[2]->m_pCore->OnAsioMessage(selector, value, message, opt);
}
