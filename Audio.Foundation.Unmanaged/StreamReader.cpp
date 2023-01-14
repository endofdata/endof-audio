#include "pch.h"
#include "StreamReader.h"

using namespace Audio::Foundation::Unmanaged;

StreamReader::StreamReader(std::istream& input) :
	m_input(input)
{
}

StreamReader::~StreamReader()
{
}

void StreamReader::Read(ISampleContainerPtr target)
{
	int samples = target->SampleCount;
	int channels = target->ChannelCount;

	if (channels == 1)
	{
		float* pDst = target->Channels[0]->SamplePtr;
		std::streamsize size = samples * sizeof(float);

		m_input.read((char*)pDst, size);
	}
	else
	{
		// HACK: This limits the maximum number of input channels to 16. But usually, we will only have one or two anyway.
		float* arDst[16];

		if (channels > 16)
		{
			channels = 16;
		}

		for (int c = 0; c < channels; c++)
		{
			arDst[c] = target->Channels[c]->SamplePtr;
		}

		for (int s = 0; s < samples; s++)
		{
			for (int c = 0; c < channels; c++)
			{
				float** ppDst = &arDst[c];

				m_input.read((char*)*ppDst, sizeof(float));

				(*ppDst)++;
			}
		}
	}
}

