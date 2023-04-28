#include "pch.h"
#include "FileWriter.h"
#include <fstream>
#include <ostream>

using namespace Audio::Foundation::Unmanaged;

FileWriter::FileWriter(const std::wstring& filename) :
	StreamWriter((std::ofstream&)m_ofstream),
	m_ofstream(std::ofstream(filename, std::ios::binary)),
	m_filename(filename)
{
}

FileWriter::~FileWriter()
{
}

const std::wstring& FileWriter::get_Filename() const
{
	return m_filename;
}
