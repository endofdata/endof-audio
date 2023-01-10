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
				/// Gets or sets the optional direct-input-monitorint output
				/// </summary>
				property IAudioOutput^ Monitor
				{
					IAudioOutput^ get();
					void set(IAudioOutput^ value);
				}

				/// <summary>
				/// Gets the output target collection
				/// </summary>
				property System::Collections::Generic::IEnumerable<IAudioOutput^>^ Targets
				{
					System::Collections::Generic::IEnumerable<IAudioOutput^>^ get();
				}

				/// <summary>
				/// Adds an audio output to which incoming audio is sent.
				/// </summary>
				/// <param name="target">Target to add</param>
				bool AddTarget(IAudioOutput^ target);

				/// <summary>
				/// Removes an audio output from the target list
				/// </summary>
				/// <param name="target">Target to remove</param>
				/// <returns><see langword="true"/> if <paramref name="target"/> has been removed successfully, otherwise <see langword="false"/></returns>
				bool RemoveTarget(IAudioOutput^ target);

				/// <summary>
				/// Removes all audio outputs from the target list
				/// </summary>
				void RemoveAllTargets();
			};
		}
	}
}
