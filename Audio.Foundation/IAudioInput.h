#pragma once

#include "IAudioOutput.h"
#include "IAudioTarget.h"

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
				/// Gets or sets the optional direct-input-monitorint output
				/// </summary>
				property IAudioOutput^ Monitor
				{
					IAudioOutput^ get();
					void set(IAudioOutput^ value);
				}


			};
		}
	}
}
