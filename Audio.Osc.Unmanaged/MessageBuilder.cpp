#include "pch.h"
#include "MessageBuilder.h"
#include <stdexcept>
#include <algorithm>

using namespace Audio::Osc::Unmanaged;

MessageBuilder::MessageBuilder() :
	m_parameters(nullptr),
	m_parametersSize(0),
	m_parametersWritePos(nullptr),
	m_parametersWritten(0),
	m_variantSizesWritten(0)
{
}

const char* MessageBuilder::get_AddressPattern() const
{
	return m_addressPattern;
}

void MessageBuilder::put_AddressPattern(const char* address)
{
	if (address == nullptr)
	{
		m_addressPattern.Clear();
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
		m_parameters = std::make_unique<char[]>(parametersSize);
		m_parametersSize = parametersSize;
		m_parametersWritePos = m_parameters.get();
	}
}

void MessageBuilder::SetNextParameter(const TypeTag tag, const void* value)
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
	m_parametersWritten++;
}

void MessageBuilder::SetAllParameters(std::istream& istr)
{
	if (m_parametersSize > 0 && m_parameters != nullptr)
	{
		istr.read(m_parameters.get(), m_parametersSize);
		m_parametersWritten = static_cast<int>(m_parameterTypes.size());
	}
}

void MessageBuilder::GetNextParameter(TypeTag& tag, const void*& value, int& size)
{
	if (ParameterCount <= m_parametersWritten)
	{
		throw std::runtime_error("No more parameters available.");
	}

	tag = ParameterType[m_parametersWritten++];

	if (IsVariantSize(tag))
	{
		if (m_variantSizesWritten >= m_variantSizes.size())
		{
			throw std::runtime_error("No more variant size parameters expected.");
		}
		else
		{
			size = m_variantSizes[m_variantSizesWritten++];
			value = m_parametersWritePos;
			m_parametersWritePos += OscString::GetPaddedStringSize(size);
		}
	}
	else
	{
		size = GetParameterSize(tag);
		value = m_parametersWritePos;
		m_parametersWritePos += size;
	}
}

int MessageBuilder::get_ParameterCount() const
{
	return static_cast<int>(m_parameterTypes.size());
}

const TypeTag MessageBuilder::get_ParameterType(int idx) const
{
	if (idx < 0 || idx >= ParameterCount)
	{
		throw std::invalid_argument("Index out of range.");
	}
	return m_parameterTypes[idx];
}

int MessageBuilder::get_Size() const
{
	return
		m_addressPattern.PaddedSize +
		OscString::GetPaddedStringSize(1 + static_cast<int>(m_parameterTypes.size())) +
		m_parametersSize;
}

std::ostream& MessageBuilder::Write(std::ostream& ostr) const
{
	int size = Size;
	ostr.write(reinterpret_cast<const char*>(&size), sizeof(size));

	return ostr << *this;
}

std::ostream& Audio::Osc::Unmanaged::operator << (std::ostream& ostr, const MessageBuilder& it)
{
	if (it.m_addressPattern.Size < 1)
	{
		throw std::runtime_error("Address pattern must be specified before sending the message.");
	}
	if (it.m_parametersWritten < it.m_parameterTypes.size())
	{
		throw std::runtime_error("All allocated parameters must be written before sending the message.");
	}

	ostr << it.m_addressPattern;

	ostr << ',';
	std::for_each(it.m_parameterTypes.begin(), it.m_parameterTypes.end(), [&ostr](const TypeTag t) { ostr << static_cast<char>(t); });

	int padSize = OscString::GetPadSize(static_cast<int>(it.m_parameterTypes.size() + 1));
	int padBytes = 0;
	ostr.write(reinterpret_cast<char*>(&padBytes), padSize);

	ostr.write(it.m_parameters.get(), it.m_parametersSize);

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

std::shared_ptr<MessageBuilder> MessageBuilder::Create(std::istream& istr)
{
	auto builder = std::make_shared<MessageBuilder>();

	istr >> builder->m_addressPattern;

	char nextChar = istr.peek();

	if (nextChar != ',')
	{
		throw std::runtime_error("Expected type tag string.");
	}

	OscString typeTags;
	istr >> typeTags;

	auto startPos = istr.tellg();
	std::vector<int> varSizes;

	const char* begin = typeTags.get() + 1;
	const char* end = begin + typeTags.Size - 1;

	builder->m_parametersSize = 0;

	std::for_each(begin, end, [&builder, &istr](char t)
	{
		if (!istr.good())
		{
			throw std::runtime_error("Failed to calculate parameter size.");
		}

		TypeTag tag = static_cast<TypeTag>(t);
		int size = 0;

		if (IsVariantSize(tag))
		{
			switch (tag)
			{
			case TypeTag::String:
			case TypeTag::Symbol:
			{
				OscString value;
				istr >> value;
				builder->m_variantSizes.push_back(value.Size);
				size = value.PaddedSize;
			}
			break;
			case TypeTag::Blob:
				int blobSize;
				istr >> blobSize;
				size = sizeof(blobSize) + blobSize;
				builder->m_variantSizes.push_back(size);
				istr.seekg(blobSize, std::ios_base::cur);
				break;
			default:
				throw std::runtime_error("Unsupported type tag.");
			}
		}
		else
		{
			size = GetParameterSize(tag);
			istr.seekg(size, std::ios_base::cur);
		}
		builder->m_parameterTypes.push_back(tag);
		builder->m_parametersSize += size;
	});

	builder->m_parameters = std::make_unique<char[]>(builder->m_parametersSize);
	builder->m_parametersWritePos = builder->m_parameters.get();
	builder->m_parametersWritten = 0;
	builder->m_variantSizesWritten = 0;

	istr.seekg(startPos);
	istr.read(builder->m_parameters.get(), builder->m_parametersSize);

	return builder;
}