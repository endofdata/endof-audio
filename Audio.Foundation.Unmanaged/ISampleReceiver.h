#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleContainer.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				/// <summary>
				/// Exposes methods to receive two channels of samples from a <see cref="ISampleContainer"/> for further processing
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("3a7970a7-fdb6-4dc6-ba80-540ab339c988")) ISampleReceiver : public IUnknown
				{
					/// <summary>
					/// Resets the sample receiver's internal status
					/// </summary>
					void Flush() = 0;

					/// <summary>
					/// Processes the current sample buffers of the <paramref name="input"/> container
					/// </summary>
					/// <param name="input">Input sample container</param>
					void Receive(ISampleContainerPtr input) = 0;
				};

				_COM_SMARTPTR_TYPEDEF(ISampleReceiver, __uuidof(ISampleReceiver));
			}
		}
	}
}
