#include "pch.h"
#include "WaveFile.h"
#include "WaveFormat.h"
#include <SampleConversionUnmanaged.h>

using namespace System;
using namespace System::IO;
using namespace Audio::Foundation::Interop;

WaveFile::WaveFile() : m_mode(Mode::Closed)
{
}

WaveFile::WaveFile(String^ fileName, Mode mode)	: m_mode(Mode::Closed)
{
	Open(fileName, mode);
}

WaveFile::WaveFile(String^ fileName, Mode mode, WaveFormat^ format) : WaveFile(fileName, mode)
{
	Format = format;
}

WaveFile::~WaveFile()
{
	Close();
}

WaveFile::Mode WaveFile::CurrentMode::get()
{
	return m_mode;
}

WaveFormat^ WaveFile::Format::get()
{
	return m_wavFormat;
}

void WaveFile::Format::set(WaveFormat^ value)
{
	if (nullptr == value)
		throw gcnew ArgumentNullException();

	m_wavFormat = value;
}

Boolean WaveFile::IsEOF::get()
{
	return m_bufferedStream->Length == m_bufferedStream->Position;
}

void WaveFile::Open(String^ fileName, Mode mode)
{
	Close();

	if (mode == Mode::Play)
		m_wavStream = gcnew FileStream(fileName, FileMode::Open, FileAccess::Read);
	else
	{
		m_wavStream = gcnew FileStream(fileName, FileMode::CreateNew, FileAccess::ReadWrite);

		WriteWavHeader(0);

		m_wavStream->Position = 0;
	}
	m_mode = mode;

	ReadWavHeader();
	m_samplesRecorded = GetTotalSampleCount();

	m_bufferedStream = gcnew System::IO::BufferedStream(m_wavStream);

	if (mode == Mode::Play)
	{
		m_binaryReader = gcnew BinaryReader(m_bufferedStream, System::Text::Encoding::Default, true);

		switch (m_wavFormat->SampleFormat)
		{
		case SampleFormat::IEEEFloat:
			m_sampleReader = gcnew SampleReader(this, &WaveFile::ReadFloat);
			break;
		case SampleFormat::PCM:
			switch (m_wavFormat->SampleSize)
			{
			case sizeof(short):
				m_sampleReader = gcnew SampleReader(this, &WaveFile::ReadPCM16);
				break;
			case sizeof(int):
				m_sampleReader = gcnew SampleReader(this, &WaveFile::ReadPCM32);
				break;
			default:
				throw gcnew NotSupportedException();
			}
			break;
		default:
			throw gcnew NotSupportedException();
		}
	}
	else
	{
		m_binaryWriter = gcnew BinaryWriter(m_bufferedStream, System::Text::Encoding::Default, true);

		switch (m_wavFormat->SampleFormat)
		{
		case SampleFormat::IEEEFloat:
			m_sampleWriter = gcnew SampleWriter(this, &WaveFile::WriteFloat);
			break;
		case SampleFormat::PCM:
			switch (m_wavFormat->SampleSize)
			{
			case sizeof(short):
				m_sampleWriter = gcnew SampleWriter(this, &WaveFile::WritePCM16);
				break;
			case sizeof(int):
				m_sampleWriter = gcnew SampleWriter(this, &WaveFile::WritePCM32);
				break;
			default:
				throw gcnew NotSupportedException();
			}
			break;
		default:
			throw gcnew NotSupportedException();
		}
	}

}

int WaveFile::ReadSamples(array<float>^ data, int offset, int count)
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int sample;

	for (sample = 0; sample < count; sample++)
	{
		if (m_binaryReader->BaseStream->Length > m_binaryReader->BaseStream->Position)
			data[offset + sample] = m_sampleReader();
		else
			break;
	}

	return sample;
}

void WaveFile::WriteSamples(array<float>^ data, int offset, int count)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	for (int sample = 0; sample < count; sample++)
		m_binaryWriter->Write(data[offset + sample]);

	m_samplesRecorded = Nullable<int>();
}

float WaveFile::ReadPCM16()
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	return Audio::Foundation::Unmanaged::SampleConversion::Int16ToFloat(m_binaryReader->ReadInt16());
}

void WaveFile::WritePCM16(float value)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}
	m_binaryWriter->Write(Audio::Foundation::Unmanaged::SampleConversion::FloatToInt16(value));
}

float WaveFile::ReadPCM32()
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	return Audio::Foundation::Unmanaged::SampleConversion::Int32ToFloat(m_binaryReader->ReadInt32());
}

void WaveFile::WritePCM32(float value)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}
	m_binaryWriter->Write(Audio::Foundation::Unmanaged::SampleConversion::FloatToInt32(value));
}

float WaveFile::ReadFloat()
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}
	return m_binaryReader->ReadSingle();
}

void WaveFile::WriteFloat(float value)
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}
	m_binaryWriter->Write(value);
}

void WaveFile::Close()
{
	if (m_mode != Mode::Closed)
	{
		m_bufferedStream->Flush();

		if (m_mode == Mode::Record)
		{
			WriteWavHeader(GetTotalSampleCount());

			delete m_binaryWriter;
			m_binaryWriter = nullptr;
		}
		else
		{
			delete m_binaryReader;
			m_binaryReader = nullptr;
		}

		delete m_bufferedStream;
		m_bufferedStream = nullptr;

		delete m_wavStream;
		m_wavStream = nullptr;

		m_mode = Mode::Closed;
	}
}

void WaveFile::ReadWavHeader()
{
	m_wavStream->Position = 0;

	m_wavFormat = gcnew WaveFormat();

	m_wavFormat->ReadToData(m_wavStream);

	m_dataOffset = m_wavStream->Position;
}

void WaveFile::WriteWavHeader(int sampleCount)
{
	long long position = m_wavStream->Position;

	m_wavStream->Position = 0;

	WaveFormat^ waveFormat = gcnew WaveFormat();

	waveFormat->SampleCount = sampleCount;
	waveFormat->SampleSize = sizeof(float);
	waveFormat->SampleFormat = SampleFormat::IEEEFloat;
	waveFormat->Channels = 1;
	waveFormat->WriteHeaderChunks(m_wavStream);

	m_wavStream->Position = position;
}

int WaveFile::GetTotalSampleCount()
{
	long long samples = 0;

	if (m_mode == Mode::Record)
	{
		if (m_samplesRecorded.HasValue)
			samples = m_samplesRecorded.Value;
		else
		{
			long long position = m_wavStream->Position;

			m_wavStream->Seek(0, SeekOrigin::End);

			samples = GetSampleCountAtCurrentPosition();

			m_wavStream->Position = position;
		}
	}
	else
		samples = m_wavFormat->SampleCount;

	return (int)samples;
}

int WaveFile::GetSampleCountAtCurrentPosition()
{
	long long bytes = m_wavStream->Position - m_dataOffset;
	long long samples = (long long)(bytes / sizeof(float));

	if (samples > Int32::MaxValue)
		throw gcnew InvalidOperationException(String::Format(
			"Recording length ({0} samples) exceeds maximum sample size per audio take ({1} samples).", samples, Int32::MaxValue));

	return (int)samples;
}

System::TimeSpan WaveFile::GetLength(double sampleRate)
{
	return SamplesToTimeSpan(sampleRate, GetTotalSampleCount());
}

System::TimeSpan WaveFile::GetPosition(double sampleRate)
{
	return SamplesToTimeSpan(sampleRate, GetSampleCountAtCurrentPosition());
}

void WaveFile::SetPosition(double sampleRate, TimeSpan value)
{
	if (value > GetLength(sampleRate))
		throw gcnew ArgumentOutOfRangeException();

	long long sampleBytes = TimeSpanToSamples(sampleRate, value) * sizeof(float);

	m_wavStream->Seek(sampleBytes + m_dataOffset, SeekOrigin::Begin);
}

long long WaveFile::TimeSpanToSamples(double sampleRate, TimeSpan value)
{
	double milliSeconds = value.TotalMilliseconds * sampleRate;

	return (long long)(milliSeconds / 1000.0f);
}

TimeSpan WaveFile::SamplesToTimeSpan(double sampleRate, long long samples)
{
	return TimeSpan::FromMilliseconds(samples / sampleRate * 1000.0f);
}
