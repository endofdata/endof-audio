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
				FileReader(const std::string& filename);
				virtual ~FileReader();

				const std::string& get_Filename() const;

				_declspec(property(get = get_Filename)) const std::string& Filename;

			private:
				const std::string& m_filename;
			};
		}
	}
}
