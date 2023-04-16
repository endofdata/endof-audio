#include "pch.h"
#include "StrConv.h"

using namespace Audio::Vst::Unmanaged;

std::wstring StrConv::ToUtf16(const char* pcszValue)
{
	int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, pcszValue, -1, NULL, 0);

	if (sizeRequired <= 0)
	{
		return std::wstring();
	}

	wchar_t* pwcszValue = reinterpret_cast<wchar_t*>(std::calloc(sizeRequired, sizeof(wchar_t)));

	if (pwcszValue == nullptr)
	{
		throw std::runtime_error("Not enough memory.");
	}
	MultiByteToWideChar(CP_UTF8, 0, pcszValue, -1, pwcszValue, sizeRequired);

	return std::wstring(pwcszValue);
}



std::string StrConv::ToUtf8(const wchar_t* pwcszValue)
{
	BOOL usedDefaultChar = FALSE;
	int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, pwcszValue, -1, NULL, 0, 0, &usedDefaultChar);

	if (sizeRequired <= 0)
	{
		return std::string();
	}

	char* pcszValue = reinterpret_cast<char*>(std::calloc(sizeRequired, sizeof(char)));

	if (pcszValue == nullptr)
	{
		throw std::runtime_error("Not enough memory.");
	}
	WideCharToMultiByte(CP_UTF8, 0, pwcszValue, -1, pcszValue, sizeRequired, 0, &usedDefaultChar);

	return std::string(pcszValue);
}


