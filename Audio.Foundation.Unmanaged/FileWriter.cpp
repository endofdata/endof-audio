#include "pch.h"
#include "FileWriter.h"
#include <fstream>
#include <ostream>

using namespace Audio::Foundation::Unmanaged;

FileWriter::FileWriter(const std::string& filename) :
	StreamWriter((std::ofstream&)std::ofstream(filename, std::ios::binary)),
	m_filename(filename)
{
}

FileWriter::~FileWriter()
{
}

const std::string& FileWriter::get_Filename() const
{
	return m_filename;
}
