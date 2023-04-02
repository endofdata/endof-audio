#pragma once

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
				/// A source for samples and starting point of a chain of <see cref="ISampleProcessor"/> instances.
				/// </summary>
				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("f897a276-8567-40c9-91e6-cfd6205f4205")) ISampleSource : public IUnknown
				{
					int ReadSamples(ISampleContainerPtr& container) = 0;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISampleSource, __uuidof(ISampleSource));
			}
		}
	}
}
