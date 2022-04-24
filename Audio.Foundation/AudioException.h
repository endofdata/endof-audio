#pragma once

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref struct AudioException : public System::Exception
			{
			public:
				AudioException();
				AudioException(System::String^ message);
				AudioException(System::String^ message, int code);
				AudioException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context);

				void GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context) override;

				property int Error
				{
					int get();
				}

			private:
				int m_error;
			};
		}
	}
}