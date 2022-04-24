#pragma once

#include "AsioError.h"

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			[System::Serializable]
			public ref class AsioInteropException : public System::Exception, public System::Runtime::Serialization::ISerializable
			{
			public:
				AsioInteropException(System::String^ message, System::Runtime::InteropServices::SEHException^ sehException);

				AsioInteropException(System::String^ message, int iErrorCode);

				[System::Security::SecurityCriticalAttribute()]
				virtual void GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, 
					System::Runtime::Serialization::StreamingContext context) override;

				property AsioError ASIOError
				{
					AsioError get();
				}
			private:
				AsioInteropException(System::Runtime::Serialization::SerializationInfo^ info, 
					System::Runtime::Serialization::StreamingContext context);

				void GetAsioErrorFromInt(int iErrorCode);

				AsioError m_asioError;
			};
		}
	}
}
