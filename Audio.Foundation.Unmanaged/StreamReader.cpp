#include "pch.h"
#include "StreamReader.h"

using namespace Audio::Foundation::Unmanaged;

StreamReader::StreamReader(std::istream& input) :
	m_input(input),
	m_isLooping(false),
	m_refCount(0)
{
}

StreamReader::~StreamReader()
{
}

IMPLEMENT_IUNKNOWN(StreamReader)

bool StreamReader::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleSource))
	{
		*ppvResult = dynamic_cast<ISampleSource*>(this);
		return true;
	}
	return false;
}


int StreamReader::ReadSamples(ISampleContainerPtr& container, bool overdub)
{
	int samples = container->SampleCount;
	int channels = container->ChannelCount;

	if (channels == 1 && !overdub)
	{
		Sample* pDst = container->Channels[0]->SamplePtr;
		std::streamsize size = samples * sizeof(Sample);

		m_input.read((char*)pDst, size);
	}
	else
	{
		// HACK: This limits the maximum number of input channels to 16. But usually, we will only have one or two anyway.
		Sample* arDst[16];

		if (channels > 16)
		{
			channels = 16;
		}

		for (int c = 0; c < channels; c++)
		{
			arDst[c] = container->Channels[c]->SamplePtr;
		}

		if (overdub)
		{
			for (int s = 0; s < samples; s++)
			{
				for (int c = 0; c < channels; c++)
				{
					Sample** ppDst = &arDst[c];
					Sample value = 0.0;

					m_input.read((char*)&value, sizeof(Sample));
					**ppDst += value;
					(*ppDst)++;
				}
			}
		}
		else
		{
			for (int s = 0; s < samples; s++)
			{
				for (int c = 0; c < channels; c++)
				{
					Sample** ppDst = &arDst[c];

					m_input.read((char*)*ppDst, sizeof(Sample));
					(*ppDst)++;
				}
			}
		}
	}
	if (m_isLooping)
	{
		if (m_input.rdstate() & std::istream::eofbit)
		{
			m_input.setstate(m_input.rdstate() & ~(std::istream::failbit | std::istream::eofbit));
			m_input.seekg(0, std::ios::beg);
		}
	}
	return samples;
}

bool StreamReader::get_IsLooping()
{
	return m_isLooping;
}

void StreamReader::put_IsLooping(bool value)
{
	m_isLooping = value;
}
