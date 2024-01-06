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