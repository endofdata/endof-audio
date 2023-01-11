#include "pch.h"
#include "FileAudioTake.h"
#include "WaveFile.h"
#include "WaveFormat.h"

using namespace System;
using namespace System::IO;
using namespace Audio::Foundation;
using namespace Audio::Foundation::Interop;

FileAudioTake::FileAudioTake(TimeSpan offset, int sampleRate, String^ wavStreamFileName)
	: AudioTakeBase(offset, sampleRate)
{
	m_fileName = wavStreamFileName;

	Prepare();
}

FileAudioTake::~FileAudioTake()
{
	CleanUp();
}

void FileAudioTake::Prepare()
{
	CleanUp();
	m_waveFile = OpenWaveFile(m_fileName);
}

void FileAudioTake::CleanUp()
{
	if (nullptr != m_waveFile)
	{
		delete m_waveFile;
		m_waveFile = nullptr;
	}
}

String^ FileAudioTake::Filename::get()
{
	return m_fileName;
}

TimeSpan FileAudioTake::Position::get()
{
	return TimeSpan::operator+(Offset, WavFile->GetPosition(SampleRate));
}

void FileAudioTake::Position::set(TimeSpan value)
{
	if (value < Offset)
		throw gcnew ArgumentOutOfRangeException();

	TimeSpan wavPosition = TimeSpan::operator-(value, Offset);

	WavFile->SetPosition(SampleRate, wavPosition);
}

TimeSpan FileAudioTake::Length::get()
{
	return WavFile->GetLength(SampleRate);
}

//int FileAudioTake::ReadNextFrame(array<Single>^ audioBuffer)
//{
//	int samples = m_waveFile->ReadSamples(audioBuffer, 0, audioBuffer->Length);
//	float lvl = Level;
//
//	for (int s = 0; s < samples; s++)
//		audioBuffer[s] *= lvl;
//
//	return samples;
//}

WaveFile^ FileAudioTake::OpenWaveFile(String^ fileName)
{
	return gcnew WaveFile(fileName);
}

WaveFile^ FileAudioTake::WavFile::get()
{
	return m_waveFile;
}

void FileAudioTake::WavFile::set(WaveFile^ value)
{
	m_waveFile = value;
}
