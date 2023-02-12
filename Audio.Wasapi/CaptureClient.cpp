#include "pch.h"
#include "CaptureClient.h"
#include "WasapiCoreException.h"

using namespace System;
using namespace Audio::Wasapi;

CaptureClient::CaptureClient(IAudioCaptureClient* pUnmanaged, WAVEFORMATEX& format) : m_pUnmanaged(pUnmanaged)
{
	m_frameSize = (format.wBitsPerSample * format.nChannels) / 8;
}

CaptureClient::~CaptureClient()
{
	CleanUp();
}

CaptureClient::!CaptureClient()
{
	CleanUp();
}

void CaptureClient::CleanUp()
{
	if (m_pUnmanaged != nullptr)
	{
		m_pUnmanaged->Release();
		m_pUnmanaged = nullptr;
	}
}

int CaptureClient::NextPacketLength::get()
{
	UINT32 packetLength = 0;

	WasapiCoreException::ThrowOnError(m_pUnmanaged->GetNextPacketSize(&packetLength), "Failed to get size of next packet");

	return (int)packetLength;
}

bool CaptureClient::Read(Action<ReadOnlySpan<byte>, int>^ callback)
{
	// Get the available data in the shared buffer.
	byte* pData = nullptr;
	UINT32 framesAvailable = 0;
	DWORD flags = 0;

	if (NextPacketLength == 0)
	{
		return false;
	}

	WasapiCoreException::ThrowOnError(m_pUnmanaged->GetBuffer(&pData, &framesAvailable, &flags, nullptr, nullptr), "Error reading next capture buffer");

	int bufferSize = framesAvailable * m_frameSize;

	if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
	{
		callback(ReadOnlySpan<byte>(), bufferSize);
	}
	else
	{
		callback(ReadOnlySpan<byte>(pData, bufferSize), bufferSize);
	}

	WasapiCoreException::ThrowOnError(m_pUnmanaged->ReleaseBuffer(framesAvailable), "Failed to release capture buffer");

	return true;
}