#pragma once

#include "AsioDebugDriverGuid.h"

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class RegisteredDriver sealed
			{
			public:
				property System::Guid ClsId
				{
					System::Guid get();
				}

				property System::String^ Name
				{
					System::String^ get();
				}

				virtual System::String^ ToString() override;

			internal:
				RegisteredDriver(System::Guid clsId, System::String^ name);

				RegisteredDriver(System::String^ clsIdString, System::String^ name);

			private:
				System::Guid _clsId;
				System::String^ _name;
			};
		}
	}
}