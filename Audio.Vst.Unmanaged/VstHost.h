#pragma once
#include <Audio.Vst.Unmanaged.h>

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			class _AUDIO_VST_UNMANAGED_API VstHost
			{
			public:
				VstHost();
				virtual ~VstHost();

				void AddLibrary(const wchar_t* pwcszPath);

			};
		}
	}
}