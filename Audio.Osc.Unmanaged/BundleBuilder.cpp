#include "pch.h"
#include "BundleBuilder.h"
#include "OscString.h"
#include "OscReader.h"

#include <algorithm>
#include <chrono>

using namespace Audio::Osc::Unmanaged;

OscString BundleBuilder::s_marker("#bundle", 7);

BundleBuilder::BundleBuilder() : BundleBuilder(BundleBuilder::GetCurrentTimeTag())
{
}

BundleBuilder::BundleBuilder(long long timeTag) : 
	m_timeTag(timeTag)
{
}

void BundleBuilder::AddPacket(std::shared_ptr<const PacketBase> packet)
{
	m_packets.push_back(packet);
}

std::ostream& Audio::Osc::Unmanaged::operator << (std::ostream& ostr, const BundleBuilder& it)
{
	ostr << BundleBuilder::s_marker;

	ostr.write(reinterpret_cast<const char*>(&it.m_timeTag), sizeof(long long));

	std::for_each(it.m_packets.begin(), it.m_packets.end(), [&ostr](const std::shared_ptr<const PacketBase>& packet) { packet->Write(ostr); });

	return ostr;
}

int BundleBuilder::get_Size() const
{
	int size =
		BundleBuilder::s_marker.PaddedSize +
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

std::shared_ptr<BundleBuilder> BundleBuilder::Create(std::istream& istr)
{
	auto builder = std::make_shared<BundleBuilder>();

	OscString marker;
	istr >> marker;

	if (marker != s_marker)
	{
		throw std::runtime_error("Expected '#bundle' marker.");
	}

	istr >> builder->m_timeTag;

	while (!istr.eof())
	{
		int elementSize;
		istr >> elementSize;

		builder->AddPacket(OscReader::ReadPacket(istr, elementSize));
	}

	return builder;
}

long long BundleBuilder::GetCurrentTimeTag()
{
	static const long long since1900 = 2208988800L;

	const auto now = std::chrono::system_clock::now();

	const std::time_t seconds = std::chrono::system_clock::to_time_t(now) + since1900;
	const std::chrono::nanoseconds nanos = now.time_since_epoch();	
	long long fraction = nanos.count() * 5;

	return seconds << 32 | fraction;
}