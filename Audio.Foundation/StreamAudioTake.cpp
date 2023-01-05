#include "pch.h"
#include "StreamAudioTake.h"

using namespace System;
using namespace System::IO;
using namespace Audio::Foundation;
using namespace Audio::Foundation::Interop;
using namespace System::Linq;

StreamAudioTake::StreamAudioTake(TimeSpan offset, int sampleRate, int sampleSize, Stream^ sampleStream)
	: StreamAudioTake(offset, sampleRate, sampleSize, sampleStream, true)
{
}

StreamAudioTake::StreamAudioTake(TimeSpan offset, int sampleRate, int sampleSize, Stream^ sampleStream, bool leaveOpen)
	: AudioTakeBase(offset, sampleRate)
{
	if (sampleStream == nullptr)
	{
		throw gcnew ArgumentNullException("Parameter 'sampleStream' cannot be null.");
	}
	m_sampleStream = sampleStream;
	m_leaveOpen = leaveOpen;
	m_bytesPerSecond = (int)(sampleSize * sampleRate);

	if (m_bytesPerSecond <= 0)
		throw gcnew InvalidOperationException("Error calculating bytes/s from core sample rate and size.");
}

StreamAudioTake::~StreamAudioTake()
{
	if (!m_leaveOpen)
	{
		delete m_sampleStream;
	}
}


Stream^ StreamAudioTake::SampleStream::get()
{
	return m_sampleStream;
}

TimeSpan StreamAudioTake::Position::get()
{
	double seconds = (double)m_sampleStream->Position / m_bytesPerSecond;

	return TimeSpan::operator+(Offset, TimeSpan::FromSeconds(seconds));
}

void StreamAudioTake::Position::set(TimeSpan value)
{
	if (value < Offset || value >= End)
		throw gcnew ArgumentOutOfRangeException();

	TimeSpan samplePosition = TimeSpan::operator-(value, Offset);

	int byteCount = (int)(samplePosition.Milliseconds * m_bytesPerSecond / 1000.0);

	m_sampleStream->Position = byteCount;
}

TimeSpan StreamAudioTake::Length::get()
{
	return TimeSpan::FromSeconds((double)m_sampleStream->Length / m_bytesPerSecond);
}

static bool IsNotZero(float f)
{
	return Math::Abs(f) > 0.00001;
}

int StreamAudioTake::ReadNextFrame(array<float>^ audioBuffer)
{
	if (m_sampleStream->Position >= m_sampleStream->Length)
	{		
		//Array::Clear(audioBuffer);
		return -1;
	}


	float vol = Level;
	float value = 0.0;
	int sampleSize = sizeof(float);
	Span<byte> span = Span<byte>((void*)&value, sampleSize);
	interior_ptr<float> pTarget = &audioBuffer[0];
	int count;

	for (count = 0; count < audioBuffer->Length; count++)
	{
		if (sampleSize > m_sampleStream->Read(span))
			break;

		*pTarget++= value * vol;
	}

	//int ignored = audioBuffer->Length - count;

	//if (ignored > 0)
	//{
	//	Array::Clear(audioBuffer, count, ignored);
	//}
	return count;
}
