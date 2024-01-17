#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleProcessor.h"
#include "ISourceJoiner.h"
#include "ISampleSource.h"
#include <vector>
#include <mutex>

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

				int get_SourceCount() const;

				ISampleSourcePtr& get_Source(const GUID& id);

				MixParameter& get_Parameter(const GUID& id);

				void AddSource(ISampleSourcePtr& source, const MixParameter& mix);

				bool RemoveSource(ISampleSourcePtr& source);

				bool RemoveSource(const GUID& id);

				void RemoveAllSources();

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				DECLARE_IUNKNOWN

			private:
				std::recursive_mutex m_processing_mutex;
				std::vector<std::pair<ISampleSourcePtr, MixParameter>> m_vecSources;
				bool m_isBypassed;
			};
		}
	}
}
