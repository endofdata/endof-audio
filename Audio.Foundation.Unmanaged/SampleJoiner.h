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
			class SampleJoiner : public ISampleJoiner, public ISampleProcessor
			{
			public:
				SampleJoiner();
				virtual ~SampleJoiner();

				ISampleContainerPtr get_Source(int index);

				void AddSource(ISampleContainerPtr& source);

				void RemoveSource(ISampleContainerPtr& source);

				void RemoveAllSources();

				ISampleProcessorPtr& get_next();
				void put_Next(ISampleProcessorPtr &value);

				bool get_HasNext();

				void Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleProcessorPtr m_pNext;
				std::vector<ISampleContainerPtr> m_vecSources;
			};
		}
	}
}
