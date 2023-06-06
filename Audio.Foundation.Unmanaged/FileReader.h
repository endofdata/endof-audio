#pragma once

#include <StreamReader.h>
#include <string>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class FileReader : public StreamReader
			{
			public:
				static FileReader* Create(const std::wstring& filename, int channelCount);

				virtual ~FileReader();

				const std::wstring& get_Filename() const;

				_declspec(property(get = get_Filename)) const std::wstring& Filename;

			private:
				FileReader(const std::wstring& filename, std::ifstream& ifstream, int channelCount);

				const std::wstring& m_filename;
				std::ifstream& m_ifStream;
			};
		}
	}
}
