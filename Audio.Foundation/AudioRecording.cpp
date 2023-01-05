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
}

WaveFile^ AudioRecording::OpenWaveFile(String^ wavStreamFileName)
{
	WaveFormat^ format = gcnew WaveFormat();

	format->Channels = 1;
	format->SampleSize = sizeof(float);
	format->SampleRate = SampleRate;
	format->SampleFormat = SampleFormat::IEEEFloat;

	WaveFile^ recording = gcnew WaveFile(wavStreamFileName, format);

	return recording;
}

void AudioRecording::WriteNextFrame(array<float>^ audioData)
{
	WavFile->WriteSamples(audioData, 0, audioData->Length);
}

void AudioRecording::Finish()
{
	WavFile->Close();

	// Reopen in play-mode
	WavFile = gcnew WaveFile(Filename);
}