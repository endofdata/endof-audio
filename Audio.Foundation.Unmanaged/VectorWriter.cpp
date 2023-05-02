#include "pch.h"
#include "VectorWriter.h"
#include "ObjectFactory.h"
#include <algorithm>
#include <functional>
#include "SampleContainer.h"
#include "SampleBuffer.h"

using namespace Audio::Foundation::Unmanaged;

VectorWriter::VectorWriter(int channelCount, int initialSize, int growth) :
	m_channelCount(channelCount),
	m_initial(initialSize),
	m_growth(growth),
	m_inUse(0),
	m_avail(0),
	m_isBypassed(false),
	m_refCount(0)
{
	if (channelCount == 0)
	{
		channelCount = 1;
	}
	if (m_initial == 0)
	{
		m_initial = 48000 * 60;
	}
	if (m_growth == 0)
	{
		m_growth = m_initial;
	}
	AllocBuffers();
}

VectorWriter::~VectorWriter()
{
	FreeBuffers();
}

IMPLEMENT_IUNKNOWN(VectorWriter)

void* VectorWriter::GetInterface(REFIID iid)
{
	if (iid == __uuidof(IUnknown))
	{
		return dynamic_cast<IUnknown*>(dynamic_cast<IRecorder*>(this));
	}
	if (iid == __uuidof(IRecorder))
	{
		return dynamic_cast<IRecorder*>(this);
	}
	if (iid == __uuidof(ISampleProcessor))
	{
		return dynamic_cast<ISampleProcessor*>(this);
	}
	return nullptr;
}

int VectorWriter::Process(ISampleContainerPtr& container, const ProcessingContext& context)
{
	if (!m_isBypassed)
	{
		int samples = container->SampleCount;
		int maxSourceChannels = container->ChannelCount;
		int channel = 0;
		int newSize = 0;
		
		if (m_avail < m_inUse + samples)
		{
			newSize = (div(m_inUse + samples, m_growth).quot + 1) * m_growth;
		}

		const std::lock_guard<std::recursive_mutex> lock(m_buffers_mutex);

		std::for_each(m_buffers.begin(), m_buffers.end(), [this, &channel, maxSourceChannels, &samples, newSize, container](Sample*& buffer)
		{
			const Sample* pSrc = container->Channels[channel]->SamplePtr;
			channel = (channel + 1) % maxSourceChannels;

			if (newSize > 0)
			{
				Sample* newBuffer = reinterpret_cast<Sample*>(std::realloc(buffer, newSize * sizeof(Sample)));

				if (newBuffer == nullptr)
				{
					m_isBypassed = true;
					samples = m_avail - m_inUse;
				}
				else
				{
					buffer = newBuffer;
				}
			}
			std::memcpy(&buffer[m_inUse], pSrc, samples * sizeof(Sample));
		});

		if (newSize > 0)
		{
			m_avail = newSize;
		}
		m_inUse += samples;

		return samples;
	}
	return 0;
}

ISampleContainerPtr VectorWriter::CreateSampleContainer(bool continueRecording, int fadeIn, int fadeOut)
{
	if (m_inUse > 0)
	{
		IsBypassed = true;

		const std::lock_guard<std::recursive_mutex> lock(m_buffers_mutex);

		FadeBuffers(fadeIn, fadeOut);

		auto container = new SampleContainer(m_buffers, m_inUse);
		m_buffers.clear();

		if (continueRecording)
		{
			IsBypassed = false;
		}

		return container;
	}
	return nullptr;
}

void VectorWriter::DropRecording(bool continueRecording)
{
	if (m_inUse > 0)
	{
		IsBypassed = true;

		const std::lock_guard<std::recursive_mutex> lock(m_buffers_mutex);

		FreeBuffers();

		if (continueRecording)
		{
			IsBypassed = false;
		}
	}
}

void VectorWriter::AllocBuffers()
{
	const std::lock_guard<std::recursive_mutex> lock(m_buffers_mutex);

	FreeBuffers();

	m_buffers.reserve(m_channelCount);

	for (int i = 0; i < m_channelCount; i++)
	{
		m_buffers.push_back(reinterpret_cast<Sample*>(std::malloc(sizeof(Sample) * m_initial)));
	}
	m_avail = m_initial;
	m_inUse = 0;
}

void VectorWriter::FreeBuffers()
{
	const std::lock_guard<std::recursive_mutex> lock(m_buffers_mutex);

	for (auto&& pSamples : m_buffers)
	{
		std::free(pSamples);
	}
	m_buffers.clear();
}

void VectorWriter::FadeBuffers(int fadeIn, int fadeOut)
{
	fadeIn = std::max(0, std::min(m_inUse / 2, fadeIn));
	fadeOut = std::max(0, std::min(m_inUse / 2, fadeOut));

	double fadeInFac = fadeIn;
	double fadeOutFac = fadeOut;

	std::function<Sample(Sample, int)> fadeInFunc = [fadeInFac](Sample sample, int index) { return sample * (double)index / fadeInFac; };
	std::function<Sample(Sample, int)> fadeOutFunc = [fadeOutFac](Sample sample, int index) { return sample * (fadeOutFac - (double)index - 1) / fadeOutFac; };

	for (Sample* buffer : m_buffers)
	{
		Sample* target = buffer;

		for (int s = 0; s < fadeIn; s++)
		{
			*target++ = fadeInFunc(*target, s);
		}

		target += m_inUse - fadeIn - fadeOut;

		for (int s = 0; s < fadeOut; s++)
		{
			*target++ = fadeOutFunc(*target, s);
		}
	}
}

bool VectorWriter::get_IsBypassed() const
{
	return m_isBypassed;
}

void VectorWriter::put_IsBypassed(bool value)
{
	if (value != m_isBypassed)
	{
		if (value == false && m_buffers.size() == 0)
		{
			const std::lock_guard<std::recursive_mutex> lock(m_buffers_mutex);

			AllocBuffers();
		}
		m_isBypassed = value;
	}
}
