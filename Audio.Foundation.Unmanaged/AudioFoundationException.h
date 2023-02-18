#pragma once

#include <Audio.Foundation.Unmanaged.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class _AUDIO_FOUNDATION_UNMANAGED_API AudioFoundationException
			{
			public:
				AudioFoundationException();
				AudioFoundationException(const char* pcszMessage);
				AudioFoundationException(const char* pcszMessage, int code);
				virtual ~AudioFoundationException();

				_declspec(property(get = get_Error)) int Error;

				int get_Error();


			private:
				char* m_pcszMessage;
				int m_error;
			};
		}
	}
}