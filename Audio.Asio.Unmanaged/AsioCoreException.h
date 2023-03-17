#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <string>

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			class _AUDIO_ASIO_UNMANAGED_API AsioCoreException
			{
			public:
				AsioCoreException();
				AsioCoreException(const char* pcszMessage);
				AsioCoreException(const char* pcszMessage, int code);
				virtual ~AsioCoreException();

				_declspec(property(get = get_Error)) int Error;

				int get_Error() const;

				_declspec(property(get = get_Message)) const char* Message;

				const char* get_Message() const;

			private:
				std::string m_message;
				int m_error;
			};
		}
	}
}