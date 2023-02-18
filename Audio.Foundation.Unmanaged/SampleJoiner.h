#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleProcessor.h"
#include "ISampleJoiner.h"
#include "ISampleContainer.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleJoiner : public Abstractions::ISampleJoiner
			{
			public:
				SampleJoiner();
				virtual ~SampleJoiner();

				virtual ISampleProcessorPtr get_Target();
				virtual void put_Target(ISampleProcessorPtr value);

				ISampleContainerPtr get_Source(int index);

				void AddSource(ISampleContainerPtr source);

				void RemoveSource(ISampleContainerPtr source);

				void RemoveAllSources();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleProcessorPtr m_pTarget;
				std::vector<ISampleContainerPtr> m_vecSources;
			};
		}
	}
}
