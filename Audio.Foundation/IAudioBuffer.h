#pragma once

namespace Audio
{
	namespace Foundation
	{
		namespace Abstractions
		{
			/// <summary>
			/// Abstraction of audio buffer for samples of type <see cref="float"/>
			/// </summary>
			public interface class IAudioBuffer
			{
				/// <summary>
				/// Gets the number of audio channels in the buffer
				/// </summary>
				property int ChannelCount
				{
					int get();
				}

				/// <summary>
				/// Gets the number of sample in the buffer
				/// </summary>
				property int SampleCount
				{
					int get();
				}

				/// <summary>
				/// Gets the pointer to the n'th channels data buffer.
				/// </summary>
				property System::IntPtr SamplePointer[int]
				{
					System::IntPtr get(int index);
				}
			};
		}
	}
}