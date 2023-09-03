#include "pch.h"
#include "MemoryBuffer.h"

using namespace Audio::Osc::Unmanaged;


MemoryBuffer::MemoryBuffer(std::istream& istr, size_t size) :
	m_buffer(std::make_unique<char>(size)),
	m_size(size)
{
	char* buffer = m_buffer.get();
	
	istr.read(buffer, size);

	setg(buffer, buffer, buffer + size);
	setp(buffer, buffer + size);
}


size_t MemoryBuffer::get_Size() const
{
	return m_size;
}