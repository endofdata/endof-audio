#include "pch.h"
#include "AudioRecording.h"
#include "WaveFile.h"

using namespace System;
using namespace System::IO;
using namespace Audio::Foundation::Interop;

AudioRecording::AudioRecording(TimeSpan offset, int sampleRate, String^ wavStreamFileName)
	: FileAudioTake(offset, sampleRate, wavStreamFileName)
{
}

WaveFile^ AudioRecording::OpenWaveFile(String^ wavStreamFileName)
{
	return gcnew WaveFile(wavStreamFileName, WaveFile::Mode::Record);
}

void AudioRecording::WriteNextFrame(array<float>^ audioData)
{
	WavFile->WriteSamples(audioData, 0, audioData->Length);
}

void AudioRecording::Finish()
{
	WavFile->Close();

	WavFile = gcnew WaveFile(Filename, WaveFile::Mode::Play);
}