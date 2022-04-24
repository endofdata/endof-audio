#include "pch.h"
#include "DeviceEnumerator.h"
#include "WasapiCoreException.h"
#include "Device.h"

using namespace Audio::Wasapi;
using namespace System;

DeviceEnumerator::DeviceEnumerator() :
    m_pUnmanaged(NULL)
{
    IMMDeviceEnumerator* pUnmanaged = NULL;
    WasapiCoreException::ThrowOnError(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pUnmanaged), gcnew String("Failed to create instance of MMDeviceEnumerator"));
    m_pUnmanaged = pUnmanaged;
}

DeviceEnumerator::~DeviceEnumerator()
{
    CleanUp();
}

DeviceEnumerator::!DeviceEnumerator()
{
    CleanUp();
}

void DeviceEnumerator::CleanUp()
{
    if (m_pUnmanaged != NULL)
    {
        m_pUnmanaged->Release();
        m_pUnmanaged = NULL;
    }
}


Device^ DeviceEnumerator::DefaultAudioEndpoint::get()
{
    IMMDevice* pUnmanaged = NULL;

    WasapiCoreException::ThrowOnError(m_pUnmanaged->GetDefaultAudioEndpoint(eCapture, eConsole, &pUnmanaged), gcnew String("Error querying default audio endpoint"));

    return gcnew Device(pUnmanaged);
}

// virtual 
System::Collections::Generic::IEnumerator<Device^>^ DeviceEnumerator::GetEnumerator()
{
    throw gcnew NotImplementedException();
}

// virtual 
System::Collections::IEnumerator^ DeviceEnumerator::GetEnumeratorNonGeneric()
{
    return GetEnumerator();
}