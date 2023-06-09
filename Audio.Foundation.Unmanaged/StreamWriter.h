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

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				DECLARE_IUNKNOWN

			private:
				std::ostream& m_output;
				bool m_isBypassed;
			};
		}
	}
}

