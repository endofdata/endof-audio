#include "pch.h"
#include "FileReader.h"
#include <fstream>
#include <istream>

using namespace Audio::Foundation::Unmanaged;

FileReader::FileReader(const std::wstring& filename, int channelCount) :
	StreamReader((std::ifstream&)m_ifstream, channelCount),
	m_ifstream(std::ifstream(filename, std::ios::binary)),
	m_filename(filename)
{
}

FileReader::~FileReader()
{
}

const std::wstring& FileReader::get_Filename() const
{
	return m_filename;
}
