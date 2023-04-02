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
				StreamReader(std::istream& input);
				virtual ~StreamReader();

				int ReadSamples(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				std::istream& m_input;
			};
		}
	}
}

