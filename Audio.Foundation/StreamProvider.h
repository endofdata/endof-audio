#pragma once

#include "SampleContainer.h"
#include "SampleSharer.h"
#include <vcclr.h>

namespace Audio
{
	namespace Foundation
	{
		public class StreamProvider : public SampleContainer, public SampleSharer
		{
		public:
			StreamProvider(int sampleCount, System::IO::Stream^ fred);

		private:
			gcroot<System::IO::Stream^> m_stream;
		};
	}
}