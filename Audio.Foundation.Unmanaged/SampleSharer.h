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

				ISampleProcessorPtr& get_Target(int iIdx);

				void AddTarget(ISampleProcessorPtr& channel);
				void RemoveTarget(ISampleProcessorPtr& channel);
				void RemoveAllTargets();

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				DECLARE_IUNKNOWN

			private:
				std::vector<ISampleProcessorPtr> m_vecTargets;
				bool m_isBypassed;
			};
		}
	}
}
