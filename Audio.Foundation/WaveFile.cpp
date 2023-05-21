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
using namespace Audio::Foundation::Unmanaged;

WaveFile::WaveFile() : m_mode(Mode::Closed)
{
	m_lock = gcnew Mutex();
	m_24BitBuffer = gcnew array<byte>(3);
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
	m_24BitBuffer = gcnew array<byte>(3);

	Create(fileName, format);
}

WaveFile::WaveFile(String^ fileName) : m_mode(Mode::Closed)
{
	if (String::IsNullOrEmpty(fileName))
	{
		throw gcnew ArgumentException("'fileName' cannot be null or empty.");
	}

	m_lock = gcnew Mutex();
	m_24BitBuffer = gcnew array<byte>(3);

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
	return m_bufferedStream->Length <= m_bufferedStream->Position;
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
				case 3:
					m_sampleWriter = gcnew SampleWriter(this, &WaveFile::WritePCM32);
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
			// VSCode 2022 cannot auto-format nested switch/case obviously
			switch (m_wavFormat->SampleSize)
			{
				case sizeof(short) :
					m_sampleReader = gcnew SampleReader(this, &WaveFile::ReadPCM16);
					break;
				case 3:
					m_sampleReader = gcnew SampleReader(this, &WaveFile::ReadPCM24);
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

int WaveFile::ReadSamples(IAudioBuffer^ target)
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int sourceChannels = Format->Channels;
	int targetChannels = target->ChannelCount;
	int done;

	if (sourceChannels == targetChannels == 1)
	{
		return ReadSamples((Sample*)target->SamplePointer[0].ToPointer(), target->SampleCount, false);
	}

	array<Sample*>^ pTargets = gcnew array<Sample*>(targetChannels);

	for (int c = 0; c < targetChannels; c++)
	{
		pTargets[c] = (Sample*)target->SamplePointer[c].ToPointer();
	}

	for (done = 0; done < target->SampleCount; done++)
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

int WaveFile::ReadSamples(array<Sample>^ target, int offset, int count, bool deInterleave)
{
	return ReadSamples(gcnew Memory<Sample>(target, offset, count), deInterleave);
}

int WaveFile::ReadSamples(Memory<Sample>^ target, bool deInterleave)
{
	MemoryHandle^ handle = target->Pin();

	try
	{
		return ReadSamples((Sample*)handle->Pointer, target->Length, deInterleave);
	}
	finally
	{
		delete handle;
	}
}

int WaveFile::ReadSamples(Sample* pTarget, int count, bool deInterleave)
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int done;
	Sample value;

	if (deInterleave && m_wavFormat->Channels > 1)
	{
		int channelCount = m_wavFormat->Channels;
		int samplesPerChannel = count / channelCount;		
		Sample** pTargets = reinterpret_cast<Sample**>(_malloca(sizeof(Sample*) * channelCount));

		try
		{
			Sample* pChannelStart = pTarget;

			for (int c = 0; c < channelCount; c++)
			{
				pTargets[c] = pChannelStart;
				pChannelStart += samplesPerChannel;
			}

			for (done = 0; done < samplesPerChannel; done++)
			{
				for (int c = 0; c < channelCount && CheckedReadSample(value); c++)
				{
					*pTargets[c]++ = value;
				}
			}
		}
		catch (const std::exception&)
		{
			_freea(pTargets);
			pTargets = nullptr;
		}
		if (pTargets != nullptr)
		{
			_freea(pTargets);
		}
	}
	else
	{
		for (done = 0; done < count && CheckedReadSample(value); done++)
		{
			*pTarget++ = value;
		}
	}
	return done;
}

int WaveFile::ReadSamples(Stream^ target, int count, bool deInterleave)
{
	if (m_mode != Mode::Play)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int done;

	BinaryWriter^ writer = gcnew BinaryWriter(target, System::Text::Encoding::Default, true);

	if (deInterleave && m_wavFormat->Channels > 1)
	{
		int channelCount = m_wavFormat->Channels;
		int samplesPerChannel = count / channelCount;
		long long startPos = m_binaryReader->BaseStream->Position;
		bool isEof = false;

		for (int c = 0; c < channelCount && !isEof; c++)
		{			
			for (done = 0; done < samplesPerChannel && !isEof; done++)
			{
				Sample value;

				for (int channel = 0; channel < channelCount && CheckedReadSample(value); channel++)
				{
					if (channel == c)
					{
						writer->Write(value);
					}
				}
			}
			m_binaryReader->BaseStream->Position = startPos;
		}
	}
	else
	{
		Sample value;

		for (done = 0; done < count && CheckedReadSample(value); done++)
		{
			writer->Write(value);
		}
	}
	return done;
}

int WaveFile::WriteSamples(IAudioBuffer^ source)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	int sourceChannels = source->ChannelCount;
	int targetChannels = Format->Channels;
	int done;

	if (sourceChannels == targetChannels == 1)
	{
		return WriteSamples((Sample*)source->SamplePointer[0].ToPointer(), source->SampleCount);
	}

	array<const Sample*>^ pSources = gcnew array<const Sample*>(sourceChannels);

	for (int c = 0; c < sourceChannels; c++)
	{
		pSources[c] = (const Sample*)source->SamplePointer[c].ToPointer();
	}

	for (done = 0; done < source->SampleCount; done++)
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

int WaveFile::WriteSamples(array<Sample>^ source, int offset, int count)
{
	return WriteSamples(ReadOnlySpan<Sample>(source, offset, count));
}

int WaveFile::WriteSamples(ReadOnlySpan<Sample> source)
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
			for each (Sample value in source)
			{
				m_sampleWriter(value);
			}
			done = source.Length;
		}
	}
	finally
	{
		m_lock->ReleaseMutex();
	}
	m_samplesRecorded = Nullable<int>();

	return done;
}

int WaveFile::WriteSamples(const Sample* pSource, int count)
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
			for (done = 0; done < count; done++)
			{
				m_sampleWriter(*pSource++);
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

int WaveFile::WriteSamples(Stream^ source, int count)
{
	if (m_mode != Mode::Record)
	{
		throw gcnew InvalidOperationException("Invalid Mode.");
	}

	m_lock->WaitOne();
	int done = 0;

	try
	{
		BinaryReader^ reader = gcnew BinaryReader(source, System::Text::Encoding::Default, true);

		try
		{
			for (int s = 0; s < count; s++)
			{
				// This must match the typedef of sample
				Sample value = reader->ReadDouble();
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

Sample WaveFile::ReadPCM16()
{
	return Unmanaged::SampleConversion::Int16ToSample(m_binaryReader->ReadInt16());
}

void WaveFile::WritePCM16(Sample value)
{
	m_binaryWriter->Write(Unmanaged::SampleConversion::SampleToInt16(value));
}

Sample WaveFile::ReadPCM24()
{
	m_binaryReader->Read(m_24BitBuffer, 0, 3);

	int nativeSample = 0;

	for (int i = 0; i < 3; i++)
	{
		nativeSample <<= 8;
		nativeSample += m_24BitBuffer[i];
	}

	return Unmanaged::SampleConversion::Int24ToSample(nativeSample);
}

void WaveFile::WritePCM24(Sample value)
{
	int nativeSample = Unmanaged::SampleConversion::SampleToInt24(value);

	for (int i = 0; i < 3; i++)
	{
		m_24BitBuffer[i] = nativeSample & 0xFF;
		nativeSample >>= 8;
	}

	m_binaryWriter->Write(m_24BitBuffer, 0, 3);
}

Sample WaveFile::ReadPCM32()
{
	return Unmanaged::SampleConversion::Int32ToSample(m_binaryReader->ReadInt32());
}

void WaveFile::WritePCM32(Sample value)
{
	m_binaryWriter->Write(Unmanaged::SampleConversion::SampleToInt32(value));
}

Sample WaveFile::ReadFloat()
{
	return Unmanaged::SampleConversion::Float32ToSample(m_binaryReader->ReadSingle());
}

void WaveFile::WriteFloat(Sample value)
{
	m_binaryWriter->Write(Unmanaged::SampleConversion::SampleToFloat32(value));
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

bool WaveFile::CheckedReadSample(Sample& value)
{
	if (!IsEOF)
	{
		value = m_sampleReader();
		return true;
	}
	return false;
}