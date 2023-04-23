#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleContainer.h>
#include <AudioTime.h>
#include <MixParameter.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				enum class AudioSeek
				{
					/// <summary>
					/// Position is absolute time value
					/// </summary>
					SeekAbsolute,
					/// <summary>
					/// Position is relative to current position
					/// </summary>
					SeekCurrent,
					/// <summary>
					/// Position is relative to take start
					/// </summary>
					SeekStart,
					/// <summary>
					/// Position is relative to take end
					/// </summary>
					SeekEnd
				};

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("933cf1fb-3110-449a-b22f-69c0b5c6e090")) ITake;

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ITake, __uuidof(ITake));

				__interface ITake : public IUnknown
				{
					int get_Id() const = 0;

					_declspec(property(get = get_Id, put = put_Id)) int Id;


					AudioTime get_Position() const = 0;
					void put_Position(AudioTime value);

					_declspec(property(get = get_Position, put = put_Position)) AudioTime Position;


					AudioTime get_Length() const = 0;
					void put_Length(AudioTime value);

					_declspec(property(get = get_Length, put = put_Length)) AudioTime Length;


					AudioTime get_EndPosition() const = 0;

					_declspec(property(get = get_EndPosition)) AudioTime EndPosition;


					ISampleContainerPtr get_Container() = 0;

					_declspec(property(get = get_Container)) ISampleContainerPtr Container;


					bool HasDataAt(AudioTime position) const = 0;

					bool SeekTo(AudioTime offset, AudioSeek kind) = 0;

					int WriteTo(ISampleContainerPtr& target, int sampleOffset, int sampleCount, int channelOffset, int channelCount, 
						int targetSampleOffset, int targetChannelOffset, const MixParameter& mix, bool overdub) const = 0;

					int WriteTo(ISampleContainerPtr& target, int channelOffset, int channelCount, int targetChannelOffset) = 0;
				};
			}
		}
	}
}
