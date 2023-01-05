#pragma once

using namespace System;

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
			public interface class IAudioTake
			{
				property System::Single Level
				{
					System::Single get();
					void set(System::Single value);
				}

				property System::TimeSpan Offset
				{
					System::TimeSpan get();
					void set(System::TimeSpan value);
				}

				property System::TimeSpan Position
				{
					System::TimeSpan get();
					void set(System::TimeSpan value);
				}

				property System::TimeSpan Length
				{
					System::TimeSpan get();
				}

				property System::TimeSpan End
				{
					System::TimeSpan get();
				}

				void Initialize();
				bool TrySetPosition(System::TimeSpan position);
				int ReadNextFrame(cli::array<float>^ audioBuffer);
			};
		}
	}
}
