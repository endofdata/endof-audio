#pragma once

#include <AsioError.h>

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class AsioInteropException : public System::Exception, public System::Runtime::Serialization::ISerializable
			{
			public:
				AsioInteropException(System::String^ message, System::Runtime::InteropServices::SEHException^ sehException);

				AsioInteropException(System::String^ message, int iErrorCode);

				property Unmanaged::AsioError ASIOError
				{
					Unmanaged::AsioError get();
				}
			private:
				void GetAsioErrorFromInt(int iErrorCode);

				Unmanaged::AsioError m_asioError;
			};
		}
	}
}
