#pragma once

#include "Audio.Osc.Unamanged.h"
#include "PacketBase.h"

#include <memory>
#include <iostream>
#include <vector>

namespace Audio
{
	namespace Osc
	{
		namespace Unmanaged
		{
			class _AUDIO_OSC_UNMANAGED_API BundleBuilder : public PacketBase
			{
			public:
				BundleBuilder(long long timeTag);

				void AddPacket(std::shared_ptr<const PacketBase> packet);

				int get_Size() const;

				std::ostream& Write(std::ostream& ostr) const;

				friend std::ostream& operator << (std::ostream& ostr, const BundleBuilder& it);

			private:
				long long m_timeTag;
				std::vector<std::shared_ptr<const PacketBase>> m_packets;
			};
		}
	}
}
