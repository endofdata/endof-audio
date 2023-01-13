#pragma once

#include <Audio.Asio.Unmanaged.h>

namespace Audio
{
	namespace Asio
	{
		class _AUDIO_ASIO_UNMANAGED_API AsioCoreException
		{
		public:
			AsioCoreException();
			AsioCoreException(const char* pcszMessage);
			AsioCoreException(const char* pcszMessage, int code);
			virtual ~AsioCoreException();

			_declspec(property(get = get_Error)) int Error;
			
			int get_Error();
			

		private:
			char* m_pcszMessage;
			int m_error;
		};
	}
}