#include "pch.h"
#include "FileReader.h"
#include <fstream>
#include <istream>

using namespace Audio::Foundation::Unmanaged;

FileReader::FileReader(const std::string& filename) :
	StreamReader((std::ifstream&)std::ifstream(filename, std::ios::binary)),
	m_filename(filename)
{
}

FileReader::~FileReader()
{
}

const std::string& FileReader::get_Filename() const
{
	return m_filename;
}
