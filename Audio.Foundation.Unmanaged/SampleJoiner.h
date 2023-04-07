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

				ISampleContainerPtr& get_Source(int index);

				void AddSource(ISampleContainerPtr& source);

				void RemoveSource(ISampleContainerPtr& source);

				void RemoveAllSources();

				bool get_IsBypassed();
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				std::vector<ISampleContainerPtr> m_vecSources;
				bool m_isBypassed;
			};
		}
	}
}
