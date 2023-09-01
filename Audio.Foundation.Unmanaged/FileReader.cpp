#include "pch.h"
#include "FileReader.h"
#include <fstream>
#include <istream>

using namespace Audio::Foundation::Unmanaged;

FileReader* FileReader::Create(const GUID& id, const std::wstring& filename, int channelCount)
{
	auto stream = new std::ifstream(filename, std::ios::binary);

	return new FileReader(id, filename, *stream, channelCount);
}

FileReader::FileReader(const GUID& id, const std::wstring& filename, std::ifstream& ifstream, int channelCount) :
	StreamReader(id, (std::istream&)ifstream, channelCount),
	m_ifStream(ifstream),
	m_filename(filename)
{
}

FileReader::~FileReader()
{
	delete& m_ifStream;
}

const std::wstring& FileReader::get_Filename() const
{
	return m_filename;
}
