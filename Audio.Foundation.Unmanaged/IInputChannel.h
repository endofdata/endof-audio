#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleProcessor.h>
#include <IOutputChannelPair.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				/// <summary>
				/// Connects an input <see cref="ISampleContainer"/> to a target <see cref="ISampleSharer"/> with optional monitoring.
				/// </summary>
				/// <remarks>
				/// Incoming hardware buffers are converted to internal sample format and distributed as two-channel sample container.
				/// </remarks>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("8a41766a-92ae-4f1d-b996-f8d4b98105ad")) IInputChannel : public IUnknown
				{
					int get_SampleType() = 0;

					/// <summary>
					/// Gets or sets the sample type provided by this input
					/// </summary>
					_declspec(property(get = get_SampleType)) int SampleType;


					ISampleProcessorPtr get_First() = 0;
					void put_First(ISampleProcessorPtr value) = 0;

					/// <summary>
					/// Gets or sets the first sample processor in the processing chain
					/// </summary>
					_declspec(property(get = get_First, put = put_First)) ISampleProcessorPtr First;


					bool get_HasFirst() = 0;

					/// <summary>
					/// Gets a value indicating whether a sample processor is available
					/// </summary>
					_declspec(property(get = get_HasFirst)) bool HasFirst;


					bool get_SupportsDirectMonitor() = 0;

					/// <summary>
					/// Gets a value indicating whether direct monitoring is supported
					/// </summary>
					_declspec(property(get = get_SupportsDirectMonitor)) bool SupportsDirectMonitor;

					IOutputChannelPairPtr get_DirectMonitor() = 0;
					void put_DirectMonitor(IOutputChannelPairPtr value) = 0;

					/// <summary>
					/// Gets or sets the optional <see cref="IOutputChannelPair"/> that is used as direct-input monitoring device.
					/// </summary>
					/// <remarks>
					/// Direct-input monitoring copies the input samples to the output buffers w/o sample conversion
					/// </remarks>
					_declspec(property(get = get_DirectMonitor, put = put_DirectMonitor)) IOutputChannelPairPtr DirectMonitor;


					bool get_HasDirectMonitor() = 0;

					/// <summary>
					/// Gets a value indicating whether a direct monitor is available
					/// </summary>
					_declspec(property(get = get_HasDirectMonitor)) bool HasDirectMonitor;


					bool get_IsActive() = 0;
					void put_IsActive(bool value) = 0;

					/// <summary>
					/// Gets or sets a value indicating whether this channel is active
					/// </summary>
					_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

					/// <summary>
					/// Processes the next audio buffer
					/// </summary>
					/// <param name="readSecondHalf">ASIO specific: is this the second half-buffer?</param>
					void OnNextBuffer(bool readSecondHalf) = 0;
				};

				_COM_SMARTPTR_TYPEDEF(IInputChannel, __uuidof(IInputChannel));
			}
		}
	}
}
