#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleContainer.h>
#include <ISampleReceiver.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				/// <summary>
				/// Handles a collection of <see cref="ISampleContainer"/> instances and routes the samples from the containers to the receivers
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("e8ed7980-1d09-46b0-9c9c-df015a1f72fe")) ISampleSharer : public IUnknown
				{
					void put_Source(ISampleContainerPtr value) = 0;
					ISampleContainerPtr get_Source() = 0;

					/// <summary>
					/// Gets or sets the sample container for the input samples
					/// </summary>
					_declspec(property(get = get_Source, put = put_Source)) ISampleContainerPtr Source;

					/// <summary>
					/// Gets targets as defined by previous calls to <see cref="AddTarget"/>
					/// </summary>
					_declspec(property(get = get_Target)) ISampleReceiverPtr Targets[];

					/// <summary>
					/// Gets one item from the <see cref="Targets"/> collection at the given <paramref name="index"/>
					/// </summary>
					/// <param name="index">Index of the requested item</param>
					/// <returns>The <see cref="ISampleReceiver"/> or <see langword="null"/> if <paramref name="index"/> is out of range.</returns>
					ISampleReceiverPtr get_Target(int index) = 0;

					/// <summary>
					/// Adds a new item to the <see cref="Targets"/> collection
					/// </summary>
					/// <param name="target">Target for output samples</param>
					/// <param name="level">Level applied to the input signal before sending to target</param>
					/// <param name="pan">Panorama position applied to the input signal before sending to target</param>
					void AddTarget(ISampleReceiverPtr target) = 0;

					/// <summary>
					/// Removes an item from the <see cref="Targets"/> collection
					/// </summary>
					/// <param name="target">Target to remove</param>
					void RemoveTarget(ISampleReceiverPtr target) = 0;

					/// <summary>
					/// Clears the <see cref="Targets"/> collection
					/// </summary>
					void RemoveAllTargets() = 0;

					/// <summary>
					/// Routes the input samples of <see cref="Source"/> to all items in the <see cref="Targets"/> collection
					/// </summary>
					void Push() = 0;
				};

				_COM_SMARTPTR_TYPEDEF(ISampleSharer, __uuidof(ISampleSharer));
			}
		}
	}
}
