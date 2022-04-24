#pragma once

#include "IAudioOutput.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
			public interface class IAudioInput
			{
				property int ChannelId
				{
					int get();
				}

				property Level DbFS
				{
					Level get();
				}

				property System::Boolean IsActive
				{
					System::Boolean get();
					void set(System::Boolean value);
				}

				property IAudioOutput^ Monitor
				{
					IAudioOutput^ get();
					void set(IAudioOutput^ value);
				}

				void ReadCurrentFrame(cli::array<System::Single>^ frameBuffer);
			};
		}
	}
}
