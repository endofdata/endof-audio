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

				void AddSource(ISampleSourcePtr& source);

				void RemoveSource(ISampleSourcePtr& source);

				void RemoveAllSources();

				bool get_IsBypassed();
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				std::vector<ISampleSourcePtr> m_vecSources;
				bool m_isBypassed;
			};
		}
	}
}
