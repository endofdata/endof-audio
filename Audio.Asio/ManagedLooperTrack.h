#pragma once

#include <MixParameter.h>

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class ManagedLooperTrack
			{
			public:
				ManagedLooperTrack(System::Guid id, Audio::Foundation::Unmanaged::MixParameter& parameter);

				property System::Guid Id;

				property double Level
				{
					double get();
					void set(double value);
				}

				property double Pan
				{
					double get();
					void set(double value);
				}

			private:
				System::Guid _id;
				Audio::Foundation::Unmanaged::MixParameter& _parameter;
			};
		}
	}
}