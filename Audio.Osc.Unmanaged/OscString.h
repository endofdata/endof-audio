#pragma once

#include "Audio.Osc.Unamanged.h"
#include <string>
#include <iostream>

namespace Audio
{
	namespace Osc
	{
		namespace Unmanaged
		{
			class _AUDIO_OSC_UNMANAGED_API OscString
			{
			public:
				OscString();
				OscString(const char* value);
				OscString(const char* value, int count);
				OscString(std::string& value);

				const char* get() const;
				
				int get_Size() const;
				_declspec(property(get = get_Size)) int Size;

				int get_PaddedSize() const;
				_declspec(property(get = get_PaddedSize)) int PaddedSize;

				bool IsEqualTo(const char* value, int count);
				void Set(const char* value, int count);
				void Clear();

				OscString& operator = (const char* value);
				bool operator == (const char* value);
				bool operator != (const char* value);

				operator const char* () const;

				std::ostream& Write(std::ostream& ostr) const;
				std::istream& Read(std::istream& istr);

				inline static int GetPaddedStringSize(int count);
				inline static int GetPadSize(int count);

			private:
				std::unique_ptr<char[]> m_value;
				int m_size;
			};

			_AUDIO_OSC_UNMANAGED_API std::ostream& operator << (std::ostream& ostr, const OscString& it);
			_AUDIO_OSC_UNMANAGED_API std::istream& operator >> (std::istream& istr, OscString& it);
		}
	}
}
