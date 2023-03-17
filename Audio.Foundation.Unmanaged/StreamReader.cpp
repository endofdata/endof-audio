#include "pch.h"
#include "StreamReader.h"

using namespace Audio::Foundation::Unmanaged;

StreamReader::StreamReader(std::istream& input, ISampleContainerPtr& container) :
	m_input(input),
	m_pContainer(container),
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

ISampleProcessorPtr& StreamReader::get_First()
{
	return m_pFirst;
}

void StreamReader::put_First(ISampleProcessorPtr& value)
{
	m_pFirst = value;
}

bool StreamReader::get_HasFirst()
{
	return m_pFirst != nullptr;
}

ISampleContainerPtr StreamReader::get_Container()
{
	return m_pContainer;
}

void StreamReader::OnNextBuffer(bool readSecondHalf)
{
	int samples = m_pContainer->SampleCount;
	int channels = m_pContainer->ChannelCount;

	if (channels == 1)
	{
		Sample* pDst = m_pContainer->Channels[0]->SamplePtr;
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
			arDst[c] = m_pContainer->Channels[c]->SamplePtr;
		}

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

