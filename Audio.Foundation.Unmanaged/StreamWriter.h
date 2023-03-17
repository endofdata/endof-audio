#pragma once

#include "ISampleProcessor.h"
#include "UnknownBase.h"
#include <ostream>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class StreamWriter : public ISampleProcessor
			{
			public:
				StreamWriter(std::ostream& output);
				virtual ~StreamWriter();

				ISampleProcessorPtr& get_next();
				void put_Next(ISampleProcessorPtr &value);

				bool get_HasNext();

				void Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleProcessorPtr m_pNext;
				std::ostream& m_output;
			};
		}
	}
}

