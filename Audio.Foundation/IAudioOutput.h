#pragma once

#include "Level.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
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
