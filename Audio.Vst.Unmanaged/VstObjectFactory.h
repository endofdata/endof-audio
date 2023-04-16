#pragma once
#include <Audio.Vst.Unmanaged.h>
#include <IVstHost.h>

using namespace Audio::Vst::Unmanaged::Abstractions;

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			class _AUDIO_VST_UNMANAGED_API VstObjectFactory
			{
			public:
				static IVstHostPtr CreateVstHost(const wchar_t* pwcszName, int sampleCount, double sampleRate);

			private:
				VstObjectFactory();
			};
		}
	}
}