#pragma once

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
			interface class IAudioTarget;

			public interface class IAudioSource
			{
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
			};
		}
	}
}
