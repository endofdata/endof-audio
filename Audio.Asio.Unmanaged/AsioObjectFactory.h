#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <ILooperConfig.h>
#include <ILooper.h>

using namespace Audio::Asio::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			class _AUDIO_ASIO_UNMANAGED_API AsioObjectFactory
			{
			public:
				static ILooperConfigPtr CreateLooperConfiguration();

				static ILooperPtr CreateLooper(const ILooperConfigPtr& config);
			};
		}
	}
}