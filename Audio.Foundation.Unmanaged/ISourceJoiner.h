#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <ISampleProcessor.h>
#include <ISampleSource.h>
#include <comdef.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{				
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("519e9d40-db0b-42c4-b435-90d7ec67206a")) ISourceJoiner : public IUnknown
				{
					/// <summary>
					/// Gets one item from the <see cref="Sources"/> collection at the given <paramref name="index"/>
					/// </summary>
					/// <param name="index">Index of the requested item</param>
					/// <returns>The <see cref="ISampleSource"/> or <see langword="null"/> if <paramref name="index"/> is out of range.</returns>
					ISampleSourcePtr& get_Source(int index) = 0;

					/// <summary>
					/// Gets sources as defined by previous calls to <see cref="AddSource"/>
					/// </summary>
					_declspec(property(get = get_Source)) ISampleSourcePtr Sources[];

					/// <summary>
					/// Adds a new item to the <see cref="Sources"/> collection
					/// </summary>
					/// <param name="source">Source to joint into output samples</param>
					void AddSource(ISampleSourcePtr& source) = 0;

					/// <summary>
					/// Removes an item from the <see cref="Sources"/> collection
					/// </summary>
					/// <param name="source">Source to remove</param>
					void RemoveSource(ISampleSourcePtr& source) = 0;

					/// <summary>
					/// Clears the <see cref="Sources"/> collection
					/// </summary>
					void RemoveAllSources() = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISourceJoiner, __uuidof(ISourceJoiner));
			}
		}
	}
}
