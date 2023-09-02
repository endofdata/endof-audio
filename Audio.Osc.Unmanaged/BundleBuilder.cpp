#include "pch.h"
#include "BundleBuilder.h"
#include <algorithm>
#include "OscString.h"

using namespace Audio::Osc::Unmanaged;

BundleBuilder::BundleBuilder(long long timeTag) : m_timeTag(timeTag)
{
}


void BundleBuilder::AddPacket(std::shared_ptr<const PacketBase> packet)
{
	m_packets.push_back(packet);
}



std::ostream& Audio::Osc::Unmanaged::operator << (std::ostream& ostr, const BundleBuilder& it)
{
	OscString::WritePadded(ostr, "#bundle");

	ostr.write(reinterpret_cast<const char*>(&it.m_timeTag), sizeof(long long));

	std::for_each(it.m_packets.begin(), it.m_packets.end(), [&ostr](const std::shared_ptr<const PacketBase>& packet) { packet->Write(ostr); });

	return ostr;
}

int BundleBuilder::get_Size() const
{
	int size =
		OscString::GetPaddedSize(std::string("#bundle")) +
		sizeof(long long);

	std::for_each(m_packets.begin(), m_packets.end(), [&size](const std::shared_ptr<const PacketBase>& packet) { size += packet->Size; });

	return size;
}

std::ostream& BundleBuilder::Write(std::ostream& ostr) const
{
	int size = get_Size();
	ostr.write(reinterpret_cast<const char*>(&size), sizeof(size));

	return ostr << *this;
}
