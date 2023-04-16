#pragma once
#include <string>
#include <Audio.Vst.Unmanaged.h>

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			class _AUDIO_VST_UNMANAGED_API StrConv
			{
			public:
				static std::wstring ToUtf16(const char* pcszValue);

				std::wstring ToUtf16(const std::string& value)
				{
					return ToUtf16(value.c_str());
				}

				static std::string ToUtf8(const wchar_t* pwcszValue);

				std::string ToUtf8(const std::wstring& value)
				{
					return ToUtf8(value.c_str());
				}

			private:
				StrConv();
			};
		}
	}
}