#pragma once

#include "Audio.Osc.Unamanged.h"
#include "PacketBase.h"

#include <string>
#include <iostream>
#include <memory>

namespace Audio
{
	namespace Osc
	{
		namespace Unmanaged
		{
			class _AUDIO_OSC_UNMANAGED_API OscReader
			{
			public:

				static std::shared_ptr<PacketBase> ReadPacket(std::istream& istr, int size);

			protected:
				std::shared_ptr<PacketBase> ReadBundle(std::istream& istr);
				std::shared_ptr<PacketBase> ReadMessage(std::istream& istr, std::string& addressPattern);

			private:
				OscReader();
			};
		}
	}
}
