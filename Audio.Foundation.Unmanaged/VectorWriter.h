#pragma once

#include "ISampleProcessor.h"
#include "UnknownBase.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class VectorWriter : public ISampleProcessor
			{
			public:
				VectorWriter(int channelCount, int initialSize, int growth);
				virtual ~VectorWriter();

				void Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleContainerPtr CreateSampleContainer();

				int m_growth;
				int m_inUse;
				std::vector<std::vector<Sample>> m_buffers;
			};
		}
	}
}