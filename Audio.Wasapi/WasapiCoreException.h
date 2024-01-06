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