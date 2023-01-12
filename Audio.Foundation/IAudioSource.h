#pragma once

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
			interface class IAudioTarget;
			interface class IAudioBuffer;

			public interface class IAudioSource
			{
				/// <summary>
				/// Gets or sets the number of samples per channel in a single sample buffer
				/// </summary>
				property int SampleCount
				{
					int get();
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
				/// Gets the output target collection
				/// </summary>
				property System::Collections::Generic::IEnumerable<IAudioTarget^>^ Targets
				{
					System::Collections::Generic::IEnumerable<IAudioTarget^>^ get();
				}

				/// <summary>
				/// Adds an audio output to which incoming audio is sent.
				/// </summary>
				/// <param name="target">Target to add</param>
				bool AddTarget(IAudioTarget^ target);

				/// <summary>
				/// Removes an audio output from the target list
				/// </summary>
				/// <param name="target">Target to remove</param>
				/// <returns><see langword="true"/> if <paramref name="target"/> has been removed successfully, otherwise <see langword="false"/></returns>
				bool RemoveTarget(IAudioTarget^ target);

				/// <summary>
				/// Removes all audio outputs from the target list
				/// </summary>
				void RemoveAllTargets();

				/// <summary>
				/// Process the next frame of samples
				/// </summary>
				/// <returns>The number of samples written to each channel of the target <paramref name="buffer"/></returns>
				int Read(IAudioBuffer^ buffer);
			};
		}
	}
}
