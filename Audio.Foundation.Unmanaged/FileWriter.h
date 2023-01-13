#pragma once

#include <StreamWriter.h>
#include <string>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class FileWriter : public StreamWriter
			{
			public:
				FileWriter(const std::string& filename);
				virtual ~FileWriter();

				const std::string& get_Filename() const;

				_declspec(property(get = get_Filename)) const std::string& Filename;

			private:
				const std::string& m_filename;
			};
		}
	}
}
