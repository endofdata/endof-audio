#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "SampleBufferSpan.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleBuffer : public SampleBufferSpan
			{
			public:
				/*! \brief Constructor

					Creates the buffers for the internal sample data

					\param[in] sampleCount		Number of samples to buffer.
				*/
				SampleBuffer(int sampleCount);

				SampleBuffer(Sample* pSamples, int count);

				/*! \brief Destructor

					Frees the internal buffer.
				*/
				virtual ~SampleBuffer();
			};
		}
	}
}
