#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleBuffer.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("4d5806ba-6683-48e6-93bb-2e6025e00226")) ISampleContainer;

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISampleContainer, __uuidof(ISampleContainer));

					/// <summary>
				/// Exposes two channels of samples as unmanaged <see cref="ISampleBuffer"/> instances
				/// </summary>
				__interface ISampleContainer : public IUnknown
				{
					virtual int get_SampleCount() const = 0;
					virtual void put_SampleCount(int sampleCount) = 0;

					/// <summary>
					/// Gets or sets the number of samples per sample buffer
					/// </summary>
					_declspec(property(get = get_SampleCount, put = put_SampleCount)) int SampleCount;

					virtual int get_ChannelCount() const = 0;
					virtual void put_ChannelCount(int channelCount) = 0;

					/// <summary>
					/// Gets or sets the number of channels
					/// </summary>
					_declspec(property(get = get_ChannelCount, put = put_ChannelCount)) int ChannelCount;

					
					virtual ISampleBufferPtr get_Channel(int index) = 0;

					/// <summary>
					/// Gets the sample buffers for the allocated channels
					/// </summary>
					_declspec(property(get = get_Channel)) ISampleBufferPtr Channels[];

					virtual int AddTo(ISampleContainerPtr other, int sampleOffset, int sampleCount, int targetOffset) const = 0;

					virtual int CopyTo(ISampleContainerPtr other, int sampleOffset, int sampleCount, int targetOffset) const = 0;

					virtual ISampleContainerPtr Span(int fromSample, int toSample) = 0;
				};

			}
		}
	}
}
