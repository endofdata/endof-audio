#pragma once

#include "Audio.Osc.Unamanged.h"
#include <iostream>

namespace Audio
{
	namespace Osc
	{
		namespace Unmanaged
		{
			class _AUDIO_OSC_UNMANAGED_API PacketBase
			{
			public:
				virtual int get_Size() const = 0;
				_declspec(property(get = get_Size)) int Size;

				virtual std::ostream& Write(std::ostream& ostr) const = 0;

				
			};
		}
	}
}
