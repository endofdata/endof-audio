#pragma once


#include "ISampleContainer.h"
#include "ISampleSource.h"
#include "UnknownBase.h"
#include "SampleContainerSpan.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class ContainerReader : public ISampleSource
			{
			public:
				ContainerReader(ISampleContainerPtr& source);
				virtual ~ContainerReader();

				int ReadSamples(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleContainerPtr m_pSource;
				int m_sampleCount;
				int m_sampleOffset;
				SampleContainerSpan m_span;
			};
		}
	}
}

