#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleSharer.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("c69caa40-5cd1-4ae1-9101-e84a503a79ec")) ISampleSource : public IUnknown
				{
					bool get_IsActive() = 0;
					void put_IsActive(bool value) = 0;

					/// <summary>
					/// Gets or sets a value indicating whether the input channel is activated
					/// </summary>
					_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

					ISampleSharerPtr get_SampleSharer() = 0;

					/// <summary>
					/// Gets the target sample sharer that distributes the input samples to a collection of <see cref="ISampleReceiver"/> instances.
					/// </summary>
					_declspec(property(get = get_SampleSharer)) ISampleSharerPtr SampleSharer;

					/// <summary>
					/// Invoked for each ASIO buffer switch
					/// </summary>
					/// <param name="readSecondHalf">Indicates which hardware buffer contains input samples. If <see langword="false"/> the first buffer is filled, otherwise the second.</param>
					/// <remarks>
					/// Distributes single channel of input samples to the <see cref="SampleContainer"/>, converting to internal sample format.
					/// </remarks>
					void Pull(bool readSecondHalf) = 0;

					/// <summary>
					/// Forwards the input data to the <see cref="SampleSharer"/> by calling <see cref="SampleSharer.Push"/>
					/// </summary>
					void Push() = 0;
				};

				_COM_SMARTPTR_TYPEDEF(ISampleSource, __uuidof(ISampleSource));
			}
		}
	}
}
