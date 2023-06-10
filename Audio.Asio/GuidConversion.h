#pragma once

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			ref class GuidConversion sealed
			{
			public:
				static System::Guid FromNative(const GUID& guid);

				static GUID FromManaged(System::Guid guid);
			};
		}
	}
}
				