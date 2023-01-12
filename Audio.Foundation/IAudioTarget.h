#pragma once

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
			interface class IAudioBuffer;

			public interface class IAudioTarget
			{
			public:
				int Write(IAudioBuffer^ data);
			};
		}
	}
}
