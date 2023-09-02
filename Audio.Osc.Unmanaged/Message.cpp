#include "pch.h"
#include "Message.h"
#include <stdexcept>
#include <algorithm>
#include "OscString.h"

using namespace Audio::Osc::Unmanaged;

MessageBuilder::MessageBuilder() :
	m_parameters(nullptr),
	m_parametersSize(0),
	m_parametersWritePos(nullptr),
	m_parametersWritten(0),
	m_variantSizesWritten(0)
{
}


void MessageBuilder::put_AddressPattern(const char* address)
{
	if (address == nullptr)
	{
		m_addressPattern.clear();
	}
	else
	{
		if (address[0] != '/')
		{
			throw std::runtime_error("Address pattern must start with a forward slash ('/').");
		}

		m_addressPattern = address;
	}
}

const char* MessageBuilder::get_AddressPattern() const
{
	return m_addressPattern.c_str();
}

void MessageBuilder::AllocParameters(const TypeTag tags[], int count)
{
	AllocParameters(tags, count, nullptr, 0);
}

void MessageBuilder::AllocParameters(const TypeTag tags[], int count, int varSize[], int varSizeCount)
{
	if (m_parameters != nullptr)
	{
		throw std::runtime_error("Parameters are already allocated.");
	}

	int parametersSize = 0;
	int varSizeIndex = 0;

	for (int i = 0; i < count; i++)
	{
		TypeTag tag = tags[i];

		m_parameterTypes.push_back(tag);

		if (IsVariantSize(tag))
		{
			// variable length types are padded to multiple of four with at least one '\0' byte
			if (varSizeIndex >= varSizeCount)
			{
				throw std::runtime_error("Not enought variant size definitions.");
			}

			int size = varSize[varSizeIndex++];
			m_variantSizes.push_back(size);

			int pad = 4 - (size % 4);
			parametersSize += size + pad;	
		}
		else
		{
			parametersSize += GetParameterSize(tag);
		}
	}
	if (parametersSize > 0)
	{
		m_parameters = new char[parametersSize];
		m_parametersSize = parametersSize;
		m_parametersWritePos = m_parameters;
	}
}

void MessageBuilder::WriteParameter(const TypeTag tag, const void* value)
{
	if (m_parametersWritten >= m_parameterTypes.size())
	{
		throw std::runtime_error("Attempt to write too many parameters.");
	}
	if (m_parameterTypes[m_parametersWritten] != tag)
	{
		throw std::runtime_error("Attempt to write unexpected parameter type.");
	}
	if (IsVariantSize(tag))
	{
		int size = m_variantSizes[m_variantSizesWritten++];
		std::memcpy(m_parametersWritePos, value, size);
		m_parametersWritePos += size;

		int pad = 4 - (size % 4);
		std::memset(m_parametersWritePos, 0, pad);
		m_parametersWritePos += pad;
	}
	else
	{
		int size = GetParameterSize(tag);
		std::memcpy(m_parametersWritePos, value, size);
		m_parametersWritePos += size;
	}
}

int MessageBuilder::get_Size() const
{
	return
		OscString::GetPaddedSize(m_addressPattern) +
		OscString::GetPaddedSize(static_cast<int>(m_parameterTypes.size() + 1)) +
		m_parametersSize;
}

std::ostream& MessageBuilder::Write(std::ostream& ostr) const
{
	int size = get_Size();
	ostr.write(reinterpret_cast<const char*>(&size), sizeof(size));

	return ostr << *this;
}


std::ostream& Audio::Osc::Unmanaged::operator << (std::ostream& ostr, const MessageBuilder& it)
{
	if (it.m_addressPattern.length() < 1)
	{
		throw std::runtime_error("Address pattern must be specified before sending the message.");
	}
	if (it.m_parametersWritten < it.m_parameterTypes.size())
	{
		throw std::runtime_error("All allocated parameters must be written before sending the message.");
	}

	OscString::WritePadded(ostr, it.m_addressPattern);

	ostr << ',';
	std::for_each(it.m_parameterTypes.begin(), it.m_parameterTypes.end(), [&ostr](const TypeTag t) { ostr << static_cast<char>(t); });

	ostr.write(it.m_parameters, it.m_parametersSize);

	return ostr;
}

bool MessageBuilder::IsVariantSize(const TypeTag tag)
{
	switch (tag)
	{
	case TypeTag::String:
	case TypeTag::Blob:
	case TypeTag::Symbol:
		return true;
	default:
		return false;
	}
}

int MessageBuilder::GetParameterSize(const TypeTag tag)
{
	switch (tag)
	{
	case TypeTag::Int32:
		return sizeof(int);
	case TypeTag::Float32:
		return sizeof(float);
	case TypeTag::BigEndianIn64:
		return sizeof(_int64);
	case TypeTag::TimeTag:
		return sizeof(_int64);
	case TypeTag::Double:
		return sizeof(double);
	case TypeTag::Char:
		// char is sent as 32 bit value
		return sizeof(int);
	case TypeTag::RGB:
		return sizeof(int);
	case TypeTag::Midi:
		return sizeof(int);
	case TypeTag::True:
	case TypeTag::False:
	case TypeTag::Nil:
	case TypeTag::Infinitum:
	case TypeTag::BeginArray:
	case TypeTag::EndArray:
		return 0;
	default:
		if (IsVariantSize(tag))
		{
			throw std::runtime_error("Cannot detect size for variant-size type.");
		}
		throw std::runtime_error("Unsupported type tag.");
	}
}
