#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleProcessor.h>

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
					/// <summary>
					/// Gets one item from the <see cref="Targets"/> collection at the given <paramref name="index"/>
					/// </summary>
					/// <param name="index">Index of the requested item</param>
					/// <returns>The <see cref="ISampleProcessor"/> or <see langword="null"/> if <paramref name="index"/> is out of range.</returns>
					ISampleProcessorPtr get_Target(int index) = 0;

					/// <summary>
					/// Gets targets as defined by previous calls to <see cref="AddTarget"/>
					/// </summary>
					_declspec(property(get = get_Target)) ISampleProcessorPtr Targets[];

					/// <summary>
					/// Adds a new item to the <see cref="Targets"/> collection
					/// </summary>
					/// <param name="target">Target for output samples</param>
					void AddTarget(ISampleProcessorPtr target) = 0;

					/// <summary>
					/// Removes an item from the <see cref="Targets"/> collection
					/// </summary>
					/// <param name="target">Target to remove</param>
					void RemoveTarget(ISampleProcessorPtr target) = 0;

					/// <summary>
					/// Clears the <see cref="Targets"/> collection
					/// </summary>
					void RemoveAllTargets() = 0;
				};

				_COM_SMARTPTR_TYPEDEF(ISampleSharer, __uuidof(ISampleSharer));
			}
		}
	}
}
