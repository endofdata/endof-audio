#include "pch.h"
#include "AudioClient.h"
#include "WasapiCoreException.h"
#include "CaptureClient.h"

using namespace System;
using namespace Audio::Wasapi;

AudioClient::AudioClient(IAudioClient* pUnmanaged) : 
	m_pUnmanaged(pUnmanaged), 
	m_pMixFormat(NULL)
{
}

AudioClient::~AudioClient()
{
	CleanUp();
}

AudioClient::!AudioClient()
{
	CleanUp();
}

void AudioClient::CleanUp()
{
	if (m_pMixFormat != NULL)
	{
		CoTaskMemFree(m_pMixFormat);
	}

	if (m_pUnmanaged != NULL)
	{
		m_pUnmanaged->Release();
		m_pUnmanaged = NULL;
	}
}

void AudioClient::Initialize()
{
	Initialize(1000);
}

void AudioClient::Initialize(double requestDurationMs)
{
	Initialize(requestDurationMs, MixFormat);
}

void AudioClient::Initialize(double requestDurationMs, WAVEFORMATEX& format)
{
	Initialize(requestDurationMs, format, false);
}

void AudioClient::Initialize(double requestDurationMs, WAVEFORMATEX& format, bool isExclusive)
{
	WasapiCoreException::ThrowOnError(m_pUnmanaged->Initialize(isExclusive ? AUDCLNT_SHAREMODE_EXCLUSIVE : AUDCLNT_SHAREMODE_SHARED, 0, requestDurationMs * REFTIMES_PER_MILLISEC, 0, &format, NULL),
		String::Format("Failed to initialize AudioClient with requestDuration {0:N} ms', channels: {1}, sample rate: {2}, sample bits: {3}, is exclusive: {4}",
			requestDurationMs, format.nChannels, format.nSamplesPerSec, format.wBitsPerSample, isExclusive));

	UpdateMixFormat();
}

CaptureClient^ AudioClient::CreateCaptureClient()
{
	IAudioCaptureClient* pUnmanaged;

	WasapiCoreException::ThrowOnError(m_pUnmanaged->GetService(__uuidof(IAudioCaptureClient), (void**)&pUnmanaged), "Failed to get service for IAudioCaptureClient");

	return gcnew CaptureClient(pUnmanaged, MixFormat);
}

void AudioClient::Start()
{
	WasapiCoreException::ThrowOnError(m_pUnmanaged->Start(), "Error starting AudioClient");
}

void AudioClient::Stop()
{
	WasapiCoreException::ThrowOnError(m_pUnmanaged->Stop(), "Error stopping AudioClient");
}

WAVEFORMATEX& AudioClient::MixFormat::get()
{
	if (m_pMixFormat == NULL)
	{
		UpdateMixFormat();
	}
	return *m_pMixFormat;
}


int AudioClient::BufferSize::get()
{
	UINT numBufferFrames = 0;
	WasapiCoreException::ThrowOnError(m_pUnmanaged->GetBufferSize(&numBufferFrames), "Error requesting buffer size");

	return (int)numBufferFrames;
}

void AudioClient::RecordingLoop(Action<ReadOnlySpan<byte>, int>^ callback)
{
	// Calculate the actual duration of the allocated buffer.
	REFERENCE_TIME hnsActualDuration = (double)REFTIMES_PER_SEC * BufferSize / MixFormat.nSamplesPerSec;
	CaptureClient^ captureClient = CreateCaptureClient();

	Start();

	bool isDone = false;

	// Sleep for half the buffer duration.
	Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

	while (captureClient->Read(callback));

	Stop();
}

void AudioClient::UpdateMixFormat()
{
	if (m_pMixFormat != NULL)
	{
		CoTaskMemFree(m_pMixFormat);
		m_pMixFormat = NULL;
	}
	WAVEFORMATEX* pDeviceFormat;
	WasapiCoreException::ThrowOnError(m_pUnmanaged->GetMixFormat(&pDeviceFormat), "Error requesting mix format");

	m_pMixFormat = pDeviceFormat;
}