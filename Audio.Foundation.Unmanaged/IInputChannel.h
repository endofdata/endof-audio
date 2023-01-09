#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>


namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface ISampleContainer;
				__interface ISampleSharer;
				__interface ISampleReceiver;
				__interface IOutputChannelPair;

				/// <summary>
				/// Connects an input <see cref="ISampleContainer"/> to a target <see cref="ISampleSharer"/> with optional monitoring.
				/// </summary>
				/// <remarks>
				/// Incoming hardware buffers are converted to internal sample format and distributed as two-channel sample container.
				/// </remarks>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("8a41766a-92ae-4f1d-b996-f8d4b98105ad")) IInputChannel : public IUnknown
				{
					bool get_IsActive() = 0;
					void put_IsActive(bool value) = 0;

					/// <summary>
					/// Gets or sets a value indicating whether the input channel is activated
					/// </summary>
					_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

					ISampleSharer& get_SampleSharer() = 0;

					/// <summary>
					/// Gets the target sample sharer that distributes the input samples to a collection of <see cref="ISampleReceiver"/> instances.
					/// </summary>
					_declspec(property(get = get_SampleSharer)) ISampleSharer& SampleSharer;

					int get_SampleType() = 0;

					/// <summary>
					/// Gets or sets the sample type provided by this input
					/// </summary>
					_declspec(property(get = get_SampleType)) int SampleType;


					IOutputChannelPair* get_DirectMonitor() = 0;
					void put_DirectMonitor(IOutputChannelPair* value) = 0;

					/// <summary>
					/// Gets or sets the optional <see cref="IOutputChannelPair"/> that is used as direct-input monitoring device.
					/// </summary>
					/// <remarks>
					/// Direct-input monitoring copies the input samples to the output buffers w/o sample conversion
					/// </remarks>
					_declspec(property(get = get_DirectMonitor, put = put_DirectMonitor)) IOutputChannelPair* DirectMonitor;

					/// <summary>
					/// Invoked for each ASIO buffer switch
					/// </summary>
					/// <param name="readSecondHalf">Indicates which hardware buffer contains input samples. If <see langword="false"/> the first buffer is filled, otherwise the second.</param>
					/// <remarks>
					/// Distributes single channel of input samples to the two-channel <see cref="SampleContainer"/>, converting to internal sample format. 
					/// Direct output of unconverted input samples to both channels of the <see cref="Monitor"/>, if any.
					/// </remarks>
					void Swap(bool readSecondHalf) = 0;

					/// <summary>
					/// Forwards the input data to the <see cref="SampleSharer"/> by calling <see cref="SampleSharer.RouteToSends"/>
					/// </summary>
					void Send() = 0;
				};
			}
		}
	}
}
