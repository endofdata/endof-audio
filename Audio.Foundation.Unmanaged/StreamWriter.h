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

				bool get_IsBypassed();
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				std::ostream& m_output;
				bool m_isBypassed;
			};
		}
	}
}

