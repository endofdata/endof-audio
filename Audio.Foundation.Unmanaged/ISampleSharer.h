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
				__interface IChannelLink;
				__interface ISampleContainer;
				__interface ISampleReceiver;

				/// <summary>
				/// Handles a collection of <see cref="IChannelLink"/> instances and routes the samples from the containers to the receivers
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("e8ed7980-1d09-46b0-9c9c-df015a1f72fe")) ISampleSharer : public IUnknown
				{
					/// <summary>
					/// Gets channel links as created by previous calls to <see cref="AddSend"/>
					/// </summary>
					_declspec(property(get = get_Send)) IChannelLink* Sends[];

					/// <summary>
					/// Gets one item from the <see cref="Sends"/> collection at the given <paramref name="index"/>
					/// </summary>
					/// <param name="index">Index of the requested item</param>
					/// <returns>The <see cref="IChannelLink"/> or <see langword="null"/> if <paramref name="index"/> is out of range.</returns>
					IChannelLink* get_Send(int index) = 0;

					/// <summary>
					/// Adds a new channel link to the <see cref="Sends"/> collection
					/// </summary>
					/// <param name="fromChannel">Container for input samples</param>
					/// <param name="toChannel">Target for output samples</param>
					/// <param name="level">Level applied to the input signal</param>
					/// <param name="pan">Panorama position applied to the input signal</param>
					void AddSend(ISampleContainer& fromChannel, ISampleReceiver& toChannel, float level, float pan) = 0;

					/// <summary>
					/// Removes a channel link from the <see cref="Sends"/> collection
					/// </summary>
					/// <param name="toChannel">Target of the channel link to remove</param>
					void RemoveSend(ISampleReceiver& toChannel) = 0;

					/// <summary>
					/// Clears the <see cref="Sends"/> collection
					/// </summary>
					void RemoveAllSends() = 0;

					/// <summary>
					/// Routes the input samples of each channel link in the <see cref="Sends"/> collection
					/// </summary>
					void RouteToSends() = 0;
				};

			}
		}
	}
}
