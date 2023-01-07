#pragma once

#include "IAudioOutput.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
			/// <summary>
			/// Abstraction of audio input providing samples of type <see cref="float"/>
			/// </summary>
			public interface class IAudioInput
			{
				/// <summary>
				/// Gets the channel number, e.g. hardware-channel index
				/// </summary>
				property int ChannelId
				{
					int get();
				}

				/// <summary>
				/// Gets the current input level
				/// </summary>
				property Level DbFS
				{
					Level get();
				}

				/// <summary>
				/// Gets or sets a value indicating whether this input is being used
				/// </summary>
				property System::Boolean IsActive
				{
					System::Boolean get();
					void set(System::Boolean value);
				}

				/// <summary>
				/// Gets or sets the optional monitoring output
				/// </summary>
				property IAudioOutput^ Monitor
				{
					IAudioOutput^ get();
					void set(IAudioOutput^ value);
				}

				/// <summary>
				/// Reads the next frame of input samples
				/// </summary>
				/// <param name="frameBuffer">Target buffer</param>
				void ReadCurrentFrame(cli::array<System::Single>^ frameBuffer);
			};
		}
	}
}
