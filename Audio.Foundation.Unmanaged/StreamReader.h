#pragma once

#include "ISampleContainer.h"
#include "SampleContainerBase.h"
#include <istream>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class StreamReader
			{
			public:
				StreamReader(std::istream& input);
				virtual ~StreamReader();

				void Read(ISampleContainerPtr target);

			private:

				std::istream& m_input;
			};
		}
	}
}

