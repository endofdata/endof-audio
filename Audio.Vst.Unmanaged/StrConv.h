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
				static wchar_t* AllocAsUtf16(const char* pcszValue);

				static std::wstring ToUtf16(const std::string& value)
				{
					auto alloced = AllocAsUtf16(value.c_str());
					if (alloced != nullptr)
					{
						// is there no way to let wstring take ownership?
						auto result = std::wstring(alloced);
						std::free(alloced);

						return result;
					}
					return std::wstring();
				}

				static char* AllocAsUtf8(const wchar_t* pwcszValue);

				static std::string ToUtf8(const std::wstring& value)
				{
					auto alloced = AllocAsUtf8(value.c_str());
					if (alloced != nullptr)
					{
						// is there no way to let string take ownership?
						auto result = std::string(alloced);
						std::free(alloced);

						return result;
					}
					return std::string();
				}

			private:
				StrConv();
			};
		}
	}
}