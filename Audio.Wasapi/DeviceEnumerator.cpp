#include "pch.h"
#include "DeviceEnumerator.h"
#include "WasapiCoreException.h"
#include "Device.h"

using namespace Audio::Wasapi;
using namespace System;

DeviceEnumerator::DeviceEnumerator() :
    m_pUnmanaged(nullptr)
{
    IMMDeviceEnumerator* pUnmanaged = nullptr;
    WasapiCoreException::ThrowOnError(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pUnmanaged), gcnew String("Failed to create instance of MMDeviceEnumerator"));
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
    if (m_pUnmanaged != nullptr)
    {
        m_pUnmanaged->Release();
        m_pUnmanaged = nullptr;
    }
}


Device^ DeviceEnumerator::DefaultAudioEndpoint::get()
{
    IMMDevice* pUnmanaged = nullptr;

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