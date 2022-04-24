#include "pch.h"
#include "SampleConversion.h"

using namespace System;
using namespace Audio::Foundation;


// static 
int SampleConversion::Int32ToFloatConverter(array<Byte>^ buffer, int count, array<Byte>^ output)
{
	if (buffer->Length < count)
		throw gcnew ArgumentException("Source buffer is too small.");

	int sampleCount = (int)(count / sizeof(int));

	if (output->Length < (int)(sizeof(float) * sampleCount))
		throw gcnew ArgumentException("Destination buffer is too small.");

	pin_ptr<unsigned char> pinnedBuffer = &buffer[0];
	pin_ptr<unsigned char> pinnedOutput = &output[0];

	int* pSrc = (int*)pinnedBuffer;
	float* pDst = (float*)pinnedOutput;

	for (int sample = 0; sample < sampleCount; sample++)
		*pDst++ = Audio::Foundation::Unmanaged::SampleConversion::Int32ToFloat(*pSrc++);

	return sampleCount;
}

// static 
int SampleConversion::Int16ToFloatConverter(array<Byte>^ buffer, int count, array<Byte>^ output)
{
	if (buffer->Length < count)
		throw gcnew ArgumentException("Source buffer is too small.");

	int sampleCount = (int)(count / sizeof(short));

	if (output->Length < (int)(sizeof(float) * sampleCount))
		throw gcnew ArgumentException("Destination buffer is too small.");

	pin_ptr<unsigned char> pinnedBuffer = &buffer[0];
	pin_ptr<unsigned char> pinnedOutput = &output[0];

	short* pSrc = (short*)pinnedBuffer;
	float* pDst = (float*)pinnedOutput;

	for (int sample = 0; sample < sampleCount; sample++)
		*pDst++ = SampleConversion::Int16ToFloat(*pSrc++);

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

