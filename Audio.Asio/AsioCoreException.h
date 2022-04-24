#pragma once

namespace Audio
{
	namespace Asio
	{
		ref struct AsioCoreException : public System::Exception
		{
		public:
			AsioCoreException();
			AsioCoreException(System::String^ message);
			AsioCoreException(System::String^ message, int code);
			AsioCoreException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context);

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