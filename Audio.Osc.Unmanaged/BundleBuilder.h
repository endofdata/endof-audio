#pragma once

#include "Audio.Osc.Unamanged.h"
#include "PacketBase.h"
#include "OscString.h"

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
				static std::shared_ptr<BundleBuilder> Create(std::istream& istr);

				BundleBuilder();
				BundleBuilder(long long timeTag);

				void AddPacket(std::shared_ptr<const PacketBase> packet);

				int get_Size() const;

				std::ostream& Write(std::ostream& ostr) const;


				static long long GetCurrentTimeTag();

				friend std::ostream& operator << (std::ostream& ostr, const BundleBuilder& it);

			private:
				static OscString s_marker;

				long long m_timeTag;
				std::vector<std::shared_ptr<const PacketBase>> m_packets;
			};
		}
	}
}
