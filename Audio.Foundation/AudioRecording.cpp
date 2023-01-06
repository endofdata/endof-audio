#include "pch.h"
#include "AudioRecording.h"
#include "WaveFile.h"
#include "WaveFormat.h"

using namespace System;
using namespace System::IO;
using namespace Audio::Foundation::Interop;

AudioRecording::AudioRecording(TimeSpan offset, int sampleRate, String^ wavStreamFileName)
	: FileAudioTake(offset, sampleRate, wavStreamFileName)
{
	// Initial capacity of 10s recording
	m_stream = gcnew MemoryStream(sampleRate * sizeof(float) * 10);
}

AudioRecording::~AudioRecording()
{
	if (m_stream != nullptr)
	{
		delete m_stream;
	}
}

WaveFile^ AudioRecording::OpenWaveFile(String^ wavStreamFileName)
{
	// Do not create the file yet
	return gcnew WaveFile();
}

void AudioRecording::WriteNextFrame(array<float>^ audioData)
{
	pin_ptr<float> pinnedData = &audioData[0];

	m_stream->Write(ReadOnlySpan<unsigned char>(pinnedData, audioData->Length * sizeof(float)));
}

bool AudioRecording::Finish()
{
	m_stream->Flush();

	int recorded = m_stream->Position / sizeof(float);

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

	return true;
}
