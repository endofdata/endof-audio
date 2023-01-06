#include "pch.h"
#include "WaveFile.h"
#include "WaveFormat.h"
#include <SampleConversionUnmanaged.h>

using namespace System;
using namespace System::IO;
using namespace System::Threading;
using namespace Audio::Foundation::Interop;

WaveFile::WaveFile() : m_mode(Mode::Closed)
{
	m_lock = gcnew Mutex();
}

WaveFile::WaveFile(String^ fileName, WaveFormat^ format) : m_mode(Mode::Closed)
{
	if (String::IsNullOrEmpty(fileName))
	{
		throw gcnew ArgumentException("'fileName' cannot be null or empty.");
	}
	if (format == nullptr)
	{
		throw gcnew ArgumentNullException("format");
	}

	m_lock = gcnew Mutex();

	Create(fileName, format);
}

WaveFile::WaveFile(String^ fileName) : m_mode(Mode::Closed)
{
	if (String::IsNullOrEmpty(fileName))
	{
		throw gcnew ArgumentException("'fileName' cannot be null or empty.");
	}

	m_lock = gcnew Mutex();

	Open(fileName);
}

WaveFile::~WaveFile()
{
	Close();
	delete m_lock;
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

void WaveFile::Create(String^ fileName, WaveFormat^ format)
{
	Close();

	m_lock->WaitOne();

	try
	{
		m_wavFormat = format;
		m_samplesRecorded = 0;
		m_mode = Mode::Record;
		m_wavStream = gcnew FileStream(fileName, FileMode::CreateNew, FileAccess::ReadWrite);

		int headerSize = WriteWavHeader(format);

		m_wavStream->Position = headerSize;

		m_bufferedStream = gcnew System::IO::BufferedStream(m_wavStream);
		m_binaryWriter = gcnew BinaryWriter(m_bufferedStream, System::Text::Encoding::Default, true);

		switch (m_wavFormat->SampleFormat)
		{
		case SampleFormat::IEEEFloat:
			m_sampleWriter = gcnew SampleWriter(this, &WaveFile::WriteFloat);
			break;
		case SampleFormat::PCM:
			// VSCode 2022 cannot auto-format nested switch/case obviously
			switch (m_wavFormat->SampleSize)
			{
				case sizeof(short) :
					m_sampleWriter = gcnew SampleWriter(this, &WaveFile::WritePCM16);
					break;
					case sizeof(int) :
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
	finally
	{
		m_lock->ReleaseMutex();
	}
}


void WaveFile::Open(String^ fileName)
{
	Close();

	m_lock->WaitOne();

	try
	{
		m_wavStream = gcnew FileStream(fileName, FileMode::Open, FileAccess::Read);
		m_mode = Mode::Play;

		ReadWavHeader();
		m_samplesRecorded = GetTotalSampleCount();

		m_bufferedStream = gcnew System::IO::BufferedStream(m_wavStream);
		m_binaryReader = gcnew BinaryReader(m_bufferedStream, System::Text::Encoding::Default, true);

		switch (m_wavFormat->SampleFormat)
		{
		case SampleFormat::IEEEFloat:
			m_sampleReader = gcnew SampleReader(this, &WaveFile::ReadFloat);
			break;
		case SampleFormat::PCM:
			switch (m_wavFormat->SampleSize)
			{
				case sizeof(short) :
					m_sampleReader = gcnew SampleReader(this, &WaveFile::ReadPCM16);
					break;
					case sizeof(int) :
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
	finally
	{
		m_lock->ReleaseMutex();
	}
}

void WaveFile::MoveToSamples()
{
	if (!m_bufferedStream->CanSeek)
	{
		throw gcnew NotSupportedException();
	}

	m_lock->WaitOne();

	m_bufferedStream->Seek(m_dataOffset, SeekOrigin::Begin);
}

void WaveFile::Close()
{
	m_lock->WaitOne();

	try
	{
		if (m_mode != Mode::Closed)
		{
			m_bufferedStream->Flush();

			if (m_mode == Mode::Record)
			{
				m_wavFormat->SampleCount = GetTotalSampleCount();
				WriteWavHeader(m_wavFormat);

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
	finally
	{
		m_lock->ReleaseMutex();
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
		{
			data[offset + sample] = m_sampleReader();
		}
		else
		{
			break;
		}
	}

	return sample;
}

void WaveFile::WriteSamples(array<float>^ data, int offset, int count)
{
	m_lock->WaitOne();

	try
	{
		if (m_mode == Mode::Record)
		{
			pin_ptr<float> pinned_data = &data[0];
			float* pDataPos = pinned_data;

			for (int sample = 0; sample < count; sample++)
			{
				m_sampleWriter(*pDataPos++);
			}
		}
	}
	finally
	{
		m_lock->ReleaseMutex();
	}
	m_samplesRecorded = Nullable<int>();

}

float WaveFile::ReadPCM16()
{
	return Audio::Foundation::Unmanaged::SampleConversion::Int16ToFloat(m_binaryReader->ReadInt16());
}

void WaveFile::WritePCM16(float value)
{
	m_binaryWriter->Write(Audio::Foundation::Unmanaged::SampleConversion::FloatToInt16(value));
}

float WaveFile::ReadPCM32()
{
	return Audio::Foundation::Unmanaged::SampleConversion::Int32ToFloat(m_binaryReader->ReadInt32());
}

void WaveFile::WritePCM32(float value)
{
	m_binaryWriter->Write(Audio::Foundation::Unmanaged::SampleConversion::FloatToInt32(value));
}

float WaveFile::ReadFloat()
{
	return m_binaryReader->ReadSingle();
}

void WaveFile::WriteFloat(float value)
{
	m_binaryWriter->Write(value);
}



int WaveFile::ReadWavHeader()
{
	m_wavStream->Position = 0;

	m_wavFormat = gcnew WaveFormat();

	int headerSize = m_wavFormat->ReadToData(m_wavStream);

	m_dataOffset = m_wavStream->Position;

	return headerSize;
}

int WaveFile::WriteWavHeader(WaveFormat^ format)
{
	m_lock->WaitOne();

	int headerSize = 0;

	try
	{
		long long position = m_wavStream->Position;

		m_wavStream->Position = 0;

		headerSize = format->WriteHeaderChunks(m_wavStream);

		m_dataOffset = m_wavStream->Position;

		m_wavStream->Position = position;
	}
	finally
	{
		m_lock->ReleaseMutex();
	}
	return headerSize;
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
