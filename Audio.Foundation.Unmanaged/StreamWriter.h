#pragma once

#include "ISampleReceiver.h"
#include "UnknownBase.h"
#include <ostream>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class StreamWriter : public ISampleReceiver
			{
			public:
				StreamWriter(std::ostream& output);
				virtual ~StreamWriter();

				void Receive(ISampleContainerPtr input);
				void Flush();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:

				std::ostream& m_output;
			};
		}
	}
}

