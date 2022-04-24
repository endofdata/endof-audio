#pragma once

#include "ISampleBuffer.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			_APIDEF class SampleBuffer : public Audio::Foundation::Unmanaged::Abstractions::ISampleBuffer
			{
			public:
				/*! \brief Constructor

					Creates the buffers for the float sample data

					\param[in] iSamples		Number of samples to buffer.
				*/
				SampleBuffer(int iSamples);

				/*! \brief Destructor

					Frees the internal buffer.
				*/
				virtual ~SampleBuffer();

				//! \brief Clear internal buffer
				void Flush();

				//! \brief Property getter for Sample
				float get_Sample(int iIdx);

				//! \brief Property setter for Sample
				void put_Sample(int iIdx, float value);

				//! \brief Property getter for SampleCount
				int get_SampleCount();

				//!\brief Property getter for SamplePtr
				float* get_SamplePtr();

			private:
				float* m_pSamples;
				int m_iSamples;
			};
		}
	}
}
