#pragma once

#ifndef _AUDIO_ASIO_UNMANAGED_API
#ifdef AUDIOASIOUNMANAGED_EXPORTS
#define _AUDIO_ASIO_UNMANAGED_API __declspec(dllexport)
#else
#define _AUDIO_ASIO_UNMANAGED_API __declspec(dllimport)
#endif
#endif

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			// max 32 bytes incl. terminating zero
			const int MaxAsioChannelName = 32;

			// max 32 bytes incl. terminating zero
			const int MaxAsioDriverName = 32;

			// max 124 bytes incl. teminating zero
			const int MaxAsioErrorMessage = 124;


		}
	}
}