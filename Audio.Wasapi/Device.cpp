#include "pch.h"
#include "Device.h"
#include "AudioClient.h"
#include "WasapiCoreException.h"

using namespace Audio::Wasapi;

Device::Device(IMMDevice* pUnmanaged) : m_pUnmanaged(pUnmanaged)
{
}

Device::~Device()
{
    CleanUp();
}

Device::!Device()
{
    CleanUp();
}

void Device::CleanUp()
{
    if (m_pUnmanaged != NULL)
    {
        m_pUnmanaged->Release();
        m_pUnmanaged = NULL;
    }
}

AudioClient^ Device::Activate()
{
    IAudioClient* pUnmanaged = NULL;

    WasapiCoreException::ThrowOnError(m_pUnmanaged->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pUnmanaged), "Cannot activate device");

    return gcnew AudioClient(pUnmanaged);
}

