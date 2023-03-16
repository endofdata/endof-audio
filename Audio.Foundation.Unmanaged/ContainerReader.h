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
				ContainerReader(ISampleContainerPtr source, int sampleCount);
				virtual ~ContainerReader();

				ISampleProcessorPtr get_First();
				void put_First(ISampleProcessorPtr value);

				bool get_HasFirst();

				void OnNextBuffer(bool readSecondHalf);

				ISampleContainerPtr get_Container();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleProcessorPtr m_pFirst;
				ISampleContainerPtr m_pSource;
				int m_sampleCount;
				int m_sampleOffset;
				SampleContainerSpan m_span;
			};
		}
	}
}

