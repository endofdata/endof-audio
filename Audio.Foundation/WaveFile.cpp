#include "pch.h"
#include "WaveFile.h"
#include "WaveFormat.h"
#include <SampleConversionUnmanaged.h>

using namespace System;
using namespace System::Buffers;
using namespace System::IO;
using namespace System::Threading;
using namespace Audio::Foundation::Abstractions;
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
		Format = format;
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

	try
	{
		m_bufferedStream->Seek(m_dataOffset, SeekOrigin::Begin);
	}
	finally
	{
		m_lock->ReleaseMutex();
	}
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

int WaveFile::ReadSamples(IAudioBuffer^ data)
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int sourceChannels = Format->Channels;
	int targetChannels = data->ChannelCount;
	int done;
	
	if (sourceChannels == targetChannels == 1)
	{
		return ReadSamples((float*)data->SamplePointer[0].ToPointer(), data->SampleCount);
	}

	array<float*>^ pTargets = gcnew array<float*>(targetChannels);

	for (int c = 0; c < targetChannels; c++)
	{
		pTargets[c] = (float*)data->SamplePointer[c].ToPointer();
	}

	for (done = 0; done < data->SampleCount; done++)
	{
		for (int c = 0; c < sourceChannels; c++)
		{
			if (c > targetChannels)
			{
				m_sampleReader();
			}
			else
			{
				*pTargets[c]++ = m_sampleReader();
			}
		}
	}
	return done;
}

int WaveFile::ReadSamples(array<float>^ data, int offset, int count)
{
	return ReadSamples(gcnew Memory<float>(data, offset, count));
}

int WaveFile::ReadSamples(Memory<float>^ data)
{
	MemoryHandle^ handle = data->Pin();

	try
	{
		return ReadSamples((float*)handle->Pointer, data->Length);
	}
	finally
	{
		delete handle;
	}
}

int WaveFile::ReadSamples(float* pData, int count)
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int done;

	for (done = 0; done < count; done++)
	{
		if (m_binaryReader->BaseStream->Length > m_binaryReader->BaseStream->Position)
		{
			*pData++ = m_sampleReader();
		}
		else
		{
			break;
		}
	}
	return done;
}

int WaveFile::ReadSamples(Stream^ output, int count)
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int done;

	BinaryWriter^ writer = gcnew BinaryWriter(output, System::Text::Encoding::Default, true);

	for (done = 0; done < count; done++)
	{
		if (m_binaryReader->BaseStream->Length > m_binaryReader->BaseStream->Position)
		{
			writer->Write(m_sampleReader());
		}
		else
		{
			break;
		}
	}
	return done;
}

int WaveFile::WriteSamples(IAudioBuffer^ data)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int sourceChannels = data->ChannelCount;
	int targetChannels = Format->Channels; 
	int done;

	if (sourceChannels == targetChannels == 1)
	{
		return WriteSamples((float*)data->SamplePointer[0].ToPointer(), data->SampleCount);
	}

	array<const float*>^ pSources = gcnew array<const float*>(sourceChannels);

	for (int c = 0; c < sourceChannels; c++)
	{
		pSources[c] = (const float*)data->SamplePointer[c].ToPointer();
	}

	for (done = 0; done < data->SampleCount; done++)
	{
		for (int c = 0; c < targetChannels; c++)
		{
			if (c > sourceChannels)
			{
				m_sampleWriter(0.0f);
			}
			else
			{
				m_sampleWriter(*pSources[c]++);
			}
		}
	}

	return done;
}

int WaveFile::WriteSamples(array<float>^ data, int offset, int count)
{
	return WriteSamples(ReadOnlySpan<float>(data, offset, count));
}

int WaveFile::WriteSamples(ReadOnlySpan<float>^ data)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	m_lock->WaitOne();
	int done = 0;

	try
	{
		if (m_mode == Mode::Record)
		{
			for each(float sample in data)
			{
				m_sampleWriter(sample);
			}
			done = data->Length;
		}
	}
	finally
	{
		m_lock->ReleaseMutex();
	}
	m_samplesRecorded = Nullable<int>();

	return done;
}

int WaveFile::WriteSamples(const float* pData, int count)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	m_lock->WaitOne();
	int done = 0;
	
	try
	{
		if (m_mode == Mode::Record)
		{
			for(done = 0; done < count; done++)
			{
				m_sampleWriter(*pData++);
			}
		}
	}
	finally
	{
		m_lock->ReleaseMutex();
	}
	m_samplesRecorded = Nullable<int>();

	return done;
}

int WaveFile::WriteSamples(Stream^ input, int count)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	m_lock->WaitOne();
	int done = 0;

	try
	{
		BinaryReader^ reader = gcnew BinaryReader(input, System::Text::Encoding::Default, true);

		try
		{
			for (int sample = 0; sample < count; sample++)
			{
				float value = reader->ReadSingle();
				m_sampleWriter(value);
				done++;
			}
		}
		finally
		{
			delete reader;
		}
	}
	finally
	{
		m_lock->ReleaseMutex();
	}
	m_samplesRecorded = Nullable<int>();

	return done;
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
