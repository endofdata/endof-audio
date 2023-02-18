#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleSharer.h"
#include <vector>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleSharer : public Audio::Foundation::Unmanaged::Abstractions::ISampleSharer
			{
			public:
				SampleSharer();
				virtual ~SampleSharer();

				virtual void put_Source(Abstractions::ISampleContainerPtr value);
				virtual Abstractions::ISampleContainerPtr get_Source();

				virtual Abstractions::ISampleProcessorPtr get_Target(int iIdx);

				virtual void AddTarget(Abstractions::ISampleProcessorPtr channel);
				virtual void RemoveTarget(Abstractions::ISampleProcessorPtr channel);
				virtual void RemoveAllTargets();

				virtual void Push();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				Abstractions::ISampleContainerPtr m_pSource;
				std::vector<Abstractions::ISampleProcessorPtr> m_vecTargets;
			};
		}
	}
}
