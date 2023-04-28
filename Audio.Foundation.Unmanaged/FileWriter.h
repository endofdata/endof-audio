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
				FileWriter(const std::wstring& filename);
				virtual ~FileWriter();

				const std::wstring& get_Filename() const;

				_declspec(property(get = get_Filename)) const std::wstring& Filename;

			private:
				const std::wstring& m_filename;
				std::ofstream m_ofstream;
			};
		}
	}
}
