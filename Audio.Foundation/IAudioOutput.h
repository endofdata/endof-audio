#pragma once

#include "Level.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
#pragma message("defining IAudioOutput")

			public interface class IAudioOutput
			{
				property int ChannelId
				{
					int get();
				}

				property Level DbFS
				{
					Level get();
				}

			};
		}
	}
}
