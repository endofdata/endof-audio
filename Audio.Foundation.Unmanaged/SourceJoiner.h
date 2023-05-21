#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleProcessor.h"
#include "ISourceJoiner.h"
#include "ISampleSource.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SourceJoiner : public ISourceJoiner, public ISampleProcessor
			{
			public:
				SourceJoiner();
				virtual ~SourceJoiner();

				ISampleSourcePtr& get_Source(int index);
				int get_SourceCount() const;
				MixParameter& get_MixParameter(int index);

				void AddSource(ISampleSourcePtr& source, const MixParameter& mix);

				void RemoveSource(ISampleSourcePtr& source);

				void RemoveAllSources();

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				DECLARE_IUNKNOWN

			private:
				std::vector<std::pair<ISampleSourcePtr, MixParameter>> m_vecSources;
				bool m_isBypassed;
			};
		}
	}
}
