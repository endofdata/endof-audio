#include "pch.h"
#include "Audio.Foundation.Unmanaged.h"
#include "AudioRecording.h"
#include "WaveFile.h"
#include "WaveFormat.h"

using namespace System;
using namespace System::IO;
using namespace System::Threading;
using namespace Audio::Foundation::Interop;
using namespace Audio::Foundation::Unmanaged;

AudioRecording::AudioRecording(TimeSpan offset, int sampleRate, String^ wavStreamFileName)
	: FileAudioTake(offset, sampleRate, wavStreamFileName)
{
	m_lock = gcnew Mutex();
	// Initial capacity of 10s recording
	m_stream = gcnew MemoryStream(sampleRate * sizeof(float) * 10);
}

AudioRecording::~AudioRecording()
{
	delete m_stream;
	delete m_lock;
}

WaveFile^ AudioRecording::OpenWaveFile(String^ wavStreamFileName)
{
	// Do not create the file yet
	return gcnew WaveFile();
}

int AudioRecording::Write(IAudioBuffer^ buffer)
{
	m_lock->WaitOne();

	try
	{
		pin_ptr<Sample> pinnedData = (Sample*)buffer->SamplePointer[0].ToPointer();

		m_stream->Write(ReadOnlySpan<unsigned char>(pinnedData, buffer->SampleCount * sizeof(Sample)));

		return buffer->SampleCount;
	}
	finally
	{
		m_lock->ReleaseMutex();
	}
}

bool AudioRecording::Finish()
{
	m_lock->WaitOne();

	try
	{
		m_stream->Flush();

		int recorded = (int)(m_stream->Position / sizeof(Sample));

		if (recorded <= 0)
		{
			return false;
		}

		WaveFormat^ format = gcnew WaveFormat();

		format->Channels = 1;
		format->SampleSize = sizeof(float);
		format->SampleRate = SampleRate;
		format->SampleFormat = SampleFormat::IEEEFloat;

		try
		{
			WavFile->Create(Filename, format);

			m_stream->Position = 0;
		
			WavFile->WriteSamples(m_stream, recorded);

			// Reopen in play-mode
			WavFile->Open(Filename);
		}
		catch (const std::exception&)
		{
			WavFile->Close();
			throw;
		}
		finally
		{
			m_stream->Position = 0;
			m_stream->SetLength(0);
		}
	}
	finally
	{
		m_lock->ReleaseMutex();
	}

	return true;
}
