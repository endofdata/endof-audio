#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleSharer.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleSharer : public ISampleSharer, public ISampleProcessor
			{
			public:
				SampleSharer();
				virtual ~SampleSharer();

				virtual ISampleProcessorPtr& get_Target(int iIdx);

				virtual void AddTarget(ISampleProcessorPtr& channel);
				virtual void RemoveTarget(ISampleProcessorPtr& channel);
				virtual void RemoveAllTargets();

				void Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				std::vector<ISampleProcessorPtr> m_vecTargets;
			};
		}
	}
}
