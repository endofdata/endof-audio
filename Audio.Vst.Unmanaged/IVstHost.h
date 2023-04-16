#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <ISampleProcessor.h>
#include <comdef.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("38755ed5-b951-4e10-8cb3-7bb26c81d35f")) IVstHost : public IUnknown
				{
					const wchar_t* AddLibrary(const wchar_t* pwcszPath) = 0;

					ISampleProcessorPtr CreateSampleProcessor(const wchar_t* pwcszPluginId) = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IVstHost, __uuidof(IVstHost));
			}
		}
	}
}
