#include "pch.h"
#include "OscReader.h"
#include "OscString.h"
#include "MessageBuilder.h"
#include "BundleBuilder.h"
#include "MemoryBuffer.h"

#include <algorithm>
#include <streambuf>
#include <istream>

using namespace Audio::Osc::Unmanaged;

std::shared_ptr<PacketBase> OscReader::ReadPacket(std::istream& istr, int size)
{
	MemoryBuffer mem(istr, size);
	std::istream mstr(&mem);
	
	switch (istr.peek())
	{
	case '#':
		return BundleBuilder::Create(mstr);
	case '/':
		return MessageBuilder::Create(mstr);
	default:
		throw std::runtime_error("Expected address pattern or '#bundle'.");
	}
}

