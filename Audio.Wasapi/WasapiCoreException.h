#pragma once

namespace Audio
{
	namespace Wasapi
	{
		ref struct WasapiCoreException : public System::Exception
		{
		public:
			WasapiCoreException();
			WasapiCoreException(System::String^ message);
			WasapiCoreException(System::String^ message, int code);
			WasapiCoreException(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context);

			void GetObjectData(System::Runtime::Serialization::SerializationInfo^ info, System::Runtime::Serialization::StreamingContext context) override;

			property int Error
			{
				int get();
			}

		internal:
			static void ThrowOnError(HRESULT hr, System::String^ message);
		private:
			int m_error;
		};
	}
}