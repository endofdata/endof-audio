#include "pch.h"
#include "SampleConversion.h"
#include <SampleConversionUnmanaged.h>

using namespace System;
using namespace Audio::Foundation;

// static 
int SampleConversion::Int32ToSampleConverter(array<Byte>^ buffer, int count, array<Byte>^ output)
{
	if (buffer->Length < count)
		throw gcnew ArgumentException("Source buffer is too small.");

	int sampleCount = (int)(count / sizeof(int));

	if (output->Length < (int)(sizeof(Unmanaged::Sample) * sampleCount))
		throw gcnew ArgumentException("Destination buffer is too small.");

	pin_ptr<unsigned char> pinnedBuffer = &buffer[0];
	pin_ptr<unsigned char> pinnedOutput = &output[0];

	int* pSrc = (int*)pinnedBuffer;
	Unmanaged::Sample* pDst = (Unmanaged::Sample*)pinnedOutput;

	for (int sample = 0; sample < sampleCount; sample++)
		*pDst++ = Audio::Foundation::Unmanaged::SampleConversion::Int32ToSample(*pSrc++);

	return sampleCount;
}

// static 
int SampleConversion::Int16ToSampleConverter(array<Byte>^ buffer, int count, array<Byte>^ output)
{
	if (buffer->Length < count)
		throw gcnew ArgumentException("Source buffer is too small.");

	int sampleCount = (int)(count / sizeof(short));

	if (output->Length < (int)(sizeof(Unmanaged::Sample) * sampleCount))
		throw gcnew ArgumentException("Destination buffer is too small.");

	pin_ptr<unsigned char> pinnedBuffer = &buffer[0];
	pin_ptr<unsigned char> pinnedOutput = &output[0];

	short* pSrc = (short*)pinnedBuffer;
	Unmanaged::Sample* pDst = (Unmanaged::Sample*)pinnedOutput;

	for (int sample = 0; sample < sampleCount; sample++)
		*pDst++ = Audio::Foundation::Unmanaged::SampleConversion::Int16ToSample(*pSrc++);

	return sampleCount;
}

// static 
int SampleConversion::Float32ToSampleConverter(array<Byte>^ buffer, int count, array<byte>^ output)
{
	if (buffer->Length < count)
		throw gcnew ArgumentException("Source buffer is too small.");

	int sampleCount = (int)(count / sizeof(float));

	if (output->Length < sampleCount)
		throw gcnew ArgumentException("Destination buffer is too small.");

	pin_ptr<unsigned char> pinnedBuffer = &buffer[0];
	pin_ptr<unsigned char> pinnedOutput = &output[0];

	float* pSrc = (float*)pinnedBuffer;
	Unmanaged::Sample* pDst = (Unmanaged::Sample*)pinnedOutput;

	for (int sample = 0; sample < sampleCount; sample++)
		*pDst++ = Audio::Foundation::Unmanaged::SampleConversion::Float32ToSample(*pSrc++);

	return sampleCount;
}

// static 
int SampleConversion::IdentityConverter(int sampleSize, array<Byte>^ buffer, int count, array<Byte>^ output)
{
	if (buffer->Length < count)
	{
		throw gcnew ArgumentException("Source buffer is too small.");
	}
	if (sampleSize <= 0)
	{
		throw gcnew ArgumentException("Sample size cannot be less than or equal to zero.");
	}

	if (buffer != output)
	{
		if (output->Length < buffer->Length)
		{
			throw gcnew ArgumentException("Destination buffer is too small.");
		}

		pin_ptr<unsigned char> pinnedBuffer = &buffer[0];
		pin_ptr<unsigned char> pinnedOutput = &output[0];

		CopyMemory(pinnedOutput, pinnedBuffer, count);
	}
	return count / sampleSize;
}

// static 
int SampleConversion::SampleToFloat32Converter(array<Byte>^ buffer, int count, array<Single>^ output)
{
	if (buffer->Length < count)
		throw gcnew ArgumentException("Source buffer is too small.");

	int sampleCount = (int)(count / sizeof(Unmanaged::Sample));

	if (output->Length < sampleCount)
		throw gcnew ArgumentException("Destination buffer is too small.");

	pin_ptr<unsigned char> pinnedBuffer = &buffer[0];
	pin_ptr<float> pinnedOutput = &output[0];

	Unmanaged::Sample* pSrc = (Unmanaged::Sample*)pinnedBuffer;
	float* pDst = (float*)pinnedOutput;

	for (int sample = 0; sample < sampleCount; sample++)
		*pDst++ = Audio::Foundation::Unmanaged::SampleConversion::SampleToFloat32(*pSrc++);

	return sampleCount;
}
