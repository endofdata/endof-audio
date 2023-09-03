#include "pch.h"
#include "OscString.h"

#include <algorithm>
#include <sstream>
#include <vector>

using namespace Audio::Osc::Unmanaged;

OscString::OscString() :
	m_value(nullptr),
	m_size(0)
{
}

OscString::OscString(const char* value)
{
	*this = value;
}

OscString::OscString(const char* value, int count)
{
	Set(value, count);
}

OscString::OscString(std::string& value) : OscString(value.c_str(), static_cast<int>(value.length()))
{
}

const char* OscString::get() const
{
	return m_value.get();
}

int OscString::get_Size() const
{
	return m_size;
}

int OscString::get_PaddedSize() const
{
	return GetPaddedStringSize(m_size);
}

OscString::operator const char* () const
{
	return get();
}

OscString& OscString::operator = (const char* value)
{
	if (value != nullptr)
	{
		Set(value, static_cast<int>(strlen(value)));
	}
	else
	{
		Clear();
	}
	return *this;
}

bool OscString::operator ==(const char* value)
{
	return IsEqualTo(value, value == nullptr ? 0 : static_cast<int>(strlen(value)));
}

bool OscString::operator !=(const char* value)
{
	return !(*this == value);
}

bool OscString::IsEqualTo(const char* value, int count)
{
	return count == m_size && (count == 0 || std::memcmp(value, m_value.get(), count) == 0);
}

void OscString::Clear()
{
	if (m_value != nullptr)
	{
		m_value.reset();
		m_size = 0;
	}
}

void OscString::Set(const char* value, int count)
{
	Clear();

	m_value = std::make_unique<char[]>(count + 1);
	m_size = count;
	std::memcpy(m_value.get(), value, count);
	m_value.get()[count] = 0;
}

std::ostream& OscString::Write(std::ostream& ostr) const
{
	int pad = 4 - (m_size % 4);
	int padBytes = 0;

	ostr.write(m_value.get(), m_size);
	return ostr.write(reinterpret_cast<char*>(&padBytes), pad);
}

std::istream& OscString::Read(std::istream& istr)
{
	Clear();

	std::vector<char> builder;
	auto startPos = istr.tellg();
	char fourChars[5];

	int size = 0;

	while (istr.good())
	{
		istr.get(fourChars, sizeof(fourChars), '\0');
		int read = static_cast<int>(std::strlen(fourChars));

		size += read;

		if (read < 4)
		{
			// clear the error flags because reading zero bytes is fully acceptable here
			istr.clear();

			m_value = std::make_unique<char[]>(size + 1);
			m_size = size;
			istr.seekg(-size, std::ios_base::cur);

			istr.read(m_value.get(), m_size);
			m_value.get()[m_size] = 0;

			istr.seekg(4 - read, std::ios_base::cur);

			if (!istr.good())
			{
				throw std::runtime_error("Failed to read string contents.");
			}

			break;
		}
	}
	return istr;
}

int OscString::GetPaddedStringSize(int count)
{
	return count + GetPadSize(count);
}

int OscString::GetPadSize(int count)
{
	return 4 - (count % 4);
}

std::ostream& Audio::Osc::Unmanaged::operator << (std::ostream& ostr, const OscString& it)
{
	return it.Write(ostr);
}

std::istream& Audio::Osc::Unmanaged::operator >> (std::istream& istr, OscString& it)
{
	return it.Read(istr);
}