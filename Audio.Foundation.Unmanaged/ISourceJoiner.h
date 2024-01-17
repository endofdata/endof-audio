#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <ISampleProcessor.h>
#include <ISampleSource.h>
#include <MixParameter.h>
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

					int get_SourceCount() const;
					_declspec(property(get = get_SourceCount)) int SourceCount;

					ISampleSourcePtr& get_Source(const GUID& id) = 0;
					_declspec(property(get = get_Source)) ISampleSourcePtr Sources[];

					MixParameter& get_Parameter(const GUID& id) = 0;
					_declspec(property(get = get_Parameter)) MixParameter Parameter[];

					/// <summary>
					/// Adds a new item to the <see cref="Sources"/> collection
					/// </summary>
					/// <param name="source">Source to joint into output samples</param>
					void AddSource(ISampleSourcePtr& source, const MixParameter& mix) = 0;

					/// <summary>
					/// Removes an item from the <see cref="Sources"/> collection
					/// </summary>
					/// <param name="source">Source to remove</param>
					bool RemoveSource(ISampleSourcePtr& source) = 0;

					/// <summary>
					/// Removes the item with the specified <paramref name="id"/> from the <see cref="Sources"/> collection
					/// </summary>
					/// <param name="id">Id of source to remove</param>
					bool RemoveSource(const GUID& id) = 0;

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
