#pragma once

#include <Audio.Foundation.Unmanaged.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{

			class _AUDIO_FOUNDATION_UNMANAGED_API WinsockLifetime
			{
			public:
				static void InitWinSock();
				static void ExitWinSock();
			private:
				static unsigned int s_winSockInitCount;
			};
		}
	}
}