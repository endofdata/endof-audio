#pragma once
#include <guiddef.h>

namespace Audio
{
	namespace Wasapi
	{
		ref class GuidConversion
		{
		public:
			static System::Guid ToManaged(GUID value);

			static GUID ToUnmanaged(System::Guid value);
		};
	}
}