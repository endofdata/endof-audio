#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleBuffer.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleBuffer : public Audio::Foundation::Unmanaged::Abstractions::ISampleBuffer
			{
			public:
				/*! \brief Constructor

					Creates the buffers for the internal sample data

					\param[in] iSamples		Number of samples to buffer.
				*/
				SampleBuffer(int iSamples);

				/*! \brief Destructor

					Frees the internal buffer.
				*/
				virtual ~SampleBuffer();

				//! \brief Clear internal buffer
				void Clear();

				//! \brief Property getter for Sample
				Sample get_Sample(int iIdx);

				//! \brief Property setter for Sample
				void put_Sample(int iIdx, Sample value);

				//! \brief Property getter for SampleCount
				int get_SampleCount();

				//!\brief Property getter for SamplePtr
				Sample* get_SamplePtr();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				Sample* m_pSamples;
				int m_iSamples;
			};
		}
	}
}
