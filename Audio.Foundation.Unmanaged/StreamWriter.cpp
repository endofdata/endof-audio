#include "pch.h"
#include "StreamWriter.h"

using namespace Audio::Foundation::Unmanaged;

StreamWriter::StreamWriter(std::ostream& output) :
	m_output(output),
	m_isBypassed(false),
	m_refCount(0)
{
}

StreamWriter::~StreamWriter()
{
	m_output.flush();
}

IMPLEMENT_IUNKNOWN(StreamWriter)

bool StreamWriter::GetInterface(REFIID iid, void** ppvResult)
{
	if (iid == __uuidof(IUnknown))
	{
		*ppvResult = dynamic_cast<IUnknown*>(this);
		return true;
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		*ppvResult = dynamic_cast<ISampleProcessor*>(this);
		return true;
	}
	return false;
}

void StreamWriter::Process(ISampleContainerPtr& container)
{
	if (!m_isBypassed)
	{
		int samples = container->SampleCount;
		int channels = container->ChannelCount;

		if (channels == 1)
		{
			const Sample* pSrc = container->Channels[0]->SamplePtr;
			std::streamsize size = samples * sizeof(Sample);

			m_output.write((const char*)pSrc, size);
		}
		else
		{
			// HACK: This limits the maximum number of input channels to 16. But usually, we will only have one or two anyway.
			const Sample* arSrc[16];

			if (channels > 16)
			{
				channels = 16;
			}

			for (int c = 0; c < channels; c++)
			{
				arSrc[c] = container->Channels[c]->SamplePtr;
			}

			for (int s = 0; s < samples; s++)
			{
				for (int c = 0; c < channels; c++)
				{
					const Sample** ppSrc = &arSrc[c];

					m_output.write((const char*)*ppSrc, sizeof(Sample));

					(*ppSrc)++;
				}
			}
		}
	}
}

bool StreamWriter::get_IsBypassed()
{
	return m_isBypassed;
}

void StreamWriter::put_IsBypassed(bool value)
{
	m_isBypassed = value;
}