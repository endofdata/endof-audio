#pragma once

#include "ISampleContainer.h"
#include "ISampleSource.h"
#include "UnknownBase.h"
#include <istream>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class StreamReader : public ISampleSource
			{
			public:
				StreamReader(std::istream& input, ISampleContainerPtr container);
				virtual ~StreamReader();

				ISampleProcessorPtr get_First();
				void put_First(ISampleProcessorPtr value);

				bool get_HasFirst();

				void OnNextBuffer(bool readSecondHalf);

				ISampleContainerPtr get_Container();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				ISampleProcessorPtr m_pFirst;
				ISampleContainerPtr m_pContainer;
				std::istream& m_input;
			};
		}
	}
}

