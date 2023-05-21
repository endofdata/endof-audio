#include "pch.h"
#include "StreamReader.h"
#include "SampleConversionUnmanaged.h"

using namespace Audio::Foundation::Unmanaged;

StreamReader::StreamReader(std::istream& input, int channelCount) :
	m_input(input),
	m_initialPos(input.tellg()),
	m_channelCount(channelCount),
	m_isLooping(false),
	m_refCount(0)
{
}

StreamReader::~StreamReader()
{
}

IMPLEMENT_IUNKNOWN(StreamReader)

void* StreamReader::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(this);
	}
	if (iid == __uuidof(ISampleSource))
	{
		return dynamic_cast<ISampleSource*>(this);
	}
	return nullptr;
}


int StreamReader::ReadSamples(ISampleContainerPtr& container, const MixParameter& mix, bool overdub)
{
	int samples = container->SampleCount;
	int channels = std::min(container->ChannelCount, m_channelCount);

	if (channels == 1 && !overdub && (mix.Level == LevelMax) && (mix.Pan == PanCenter))
	{
		Sample* pDst = container->Channels[0]->SamplePtr;
		std::streamsize size = samples * sizeof(Sample);

		m_input.read((char*)pDst, size);
	}
	else
	{
		std::vector<Sample*> arDst;
		arDst.reserve(channels);

		for (int c = 0; c < channels && c < m_channelCount; c++)
		{
			arDst.push_back(container->Channels[c]->SamplePtr);
		}

		std::function<Sample* (const Sample& source, Sample* target, double channelFactor)> handler = overdub ?
			[](const Sample& source, Sample* target, double channelFactor) { *target++ += static_cast<Sample>(source * channelFactor); return target; } :
			[](const Sample& source, Sample* target, double channelFactor) { *target++ = static_cast<Sample>(source * channelFactor); return target;  };

		for (int s = 0; s < samples; s++)
		{
			for (int c = 0; c < channels; c++)
			{
				Sample** ppDst = &arDst[c];
				Sample value = 0.0;
				
				m_input.read((char*)&value, sizeof(Sample));
				*ppDst = handler(value, *ppDst, (c & 1)? mix.FactorRight : mix.FactorLeft);

				if (m_input.rdstate() & std::istream::eofbit)
				{
					if (m_isLooping)
					{
						m_input.setstate(m_input.rdstate() & ~(std::istream::failbit | std::istream::eofbit));
						SamplePosition = 0;
					}
					else
					{
						return s;
					}
				}
			}
		}
	}
	if (m_isLooping)
	{
		if (m_input.rdstate() & std::istream::eofbit)
		{
			m_input.setstate(m_input.rdstate() & ~(std::istream::failbit | std::istream::eofbit));
			SamplePosition = 0;
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

int StreamReader::get_ChannelCount() const
{
	return m_channelCount;
}

void StreamReader::put_ChannelCount(int value)
{
	m_channelCount = value;
}

int StreamReader::get_SamplePosition() const
{
	std::streampos pos = m_input.tellg() - m_initialPos;

	return (int)(pos / sizeof(Sample) / m_channelCount);
}

void StreamReader::put_SamplePosition(int value)
{
	std::streampos pos = value * sizeof(Sample) * m_channelCount;

	m_input.seekg(m_initialPos + pos, std::ios::beg);
}

