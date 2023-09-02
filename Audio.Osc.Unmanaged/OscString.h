#pragma once

#include "Audio.Osc.Unamanged.h"
#include <string>
#include <iostream>

namespace Audio
{
	namespace Osc
	{
		namespace Unmanaged
		{
			class _AUDIO_OSC_UNMANAGED_API OscString
			{
			public:
				static int GetPaddedSize(const std::string& value);
				static int GetPaddedSize(int count);

				static std::ostream& WritePadded(std::ostream& ostr, const std::string& value);
				static std::ostream& WritePadded(std::ostream& ostr, const char* value, int count);

			private:
				OscString();
			};
		}
	}
}
