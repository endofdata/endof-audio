#pragma once

#include "IAudioTarget.h"

using namespace Audio::Foundation::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Interop
		{
			public ref class AudioTargetBase abstract : IAudioTarget
			{
			public:
				AudioTargetBase();
				virtual ~AudioTargetBase();
			};
		}
	}
}
