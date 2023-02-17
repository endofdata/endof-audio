#include "pch.h"
#include "StreamWriter.h"

using namespace Audio::Foundation::Unmanaged;

StreamWriter::StreamWriter(std::ostream& output) :
	m_output(output),
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
		*ppvResult = dynamic_cast<IUnknown*>(dynamic_cast<ISampleReceiver*>(this));
		return true;
	}
	if (iid == __uuidof(ISampleReceiver))
	{
		*ppvResult = dynamic_cast<ISampleReceiver*>(this);
		return true;
	}
	return false;
}

void StreamWriter::Receive(ISampleContainerPtr input)
{
	int samples = input->SampleCount;
	int channels = input->ChannelCount;

	if (channels == 1)
	{
		const sample* pSrc = input->Channels[0]->SamplePtr;
		std::streamsize size = samples * sizeof(sample);

		m_output.write((const char*)pSrc, size);
	}
	else
	{
		// HACK: This limits the maximum number of input channels to 16. But usually, we will only have one or two anyway.
		const sample* arSrc[16];

		if (channels > 16)
		{
			channels = 16;
		}

		for (int c = 0; c < channels; c++)
		{
			arSrc[c] = input->Channels[c]->SamplePtr;
		}

		for (int s = 0; s < samples; s++)
		{
			for (int c = 0; c < channels; c++)
			{
				const sample** ppSrc = &arSrc[c];

				m_output.write((const char*)*ppSrc, sizeof(sample));

				(*ppSrc)++;
			}
		}
	}
}

void StreamWriter::Flush()
{
}


