#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleProcessor.h>
#include <MixParameter.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				/// <summary>
				/// A source for samples and starting point of a chain of <see cref="ISampleProcessor"/> instances.
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("f897a276-8567-40c9-91e6-cfd6205f4205")) ISampleSource : public IUnknown
				{
					int ReadSamples(ISampleContainerPtr& container, const MixParameter& mix, bool overdub) = 0;

					bool get_IsLooping() = 0;
					void put_IsLooping(bool value) = 0;
					_declspec(property(get = get_IsLooping, put = put_IsLooping)) bool IsLooping;

					int get_SamplePosition() const = 0;
					void put_SamplePosition(int value) = 0;
					_declspec(property(get = get_SamplePosition, put = put_SamplePosition)) int SamplePosition;

					int get_ChannelCount() const = 0;
					void put_ChannelCount(int value) = 0;
					_declspec(property(get = get_ChannelCount, put = put_ChannelCount)) int ChannelCount;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISampleSource, __uuidof(ISampleSource));
			}
		}
	}
}
