#include "pch.h"
#include "OscString.h"

using namespace Audio::Osc::Unmanaged;


int OscString::GetPaddedSize(const std::string& value)
{
	return GetPaddedSize(static_cast<int>(value.length()));
}

int OscString::GetPaddedSize(int count)
{
	int pad = 4 - (count % 4);

	return count + pad;
}


std::ostream& OscString::WritePadded(std::ostream& ostr, const std::string& value)
{
	return WritePadded(ostr, value.c_str(), static_cast<int>(value.length()));
}

std::ostream& OscString::WritePadded(std::ostream& ostr, const char* value, int count)
{
	int pad = 4 - (count % 4);
	int padBytes = 0;

	ostr.write(value, count);
	return ostr.write(reinterpret_cast<char*>(&padBytes), pad);
}

