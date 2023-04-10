#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleContainer.h>
#include <ProcessingContext.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{				
				__interface __declspec(uuid("c69caa40-5cd1-4ae1-9101-e84a503a79ec")) ISampleProcessor;

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISampleProcessor, __uuidof(ISampleProcessor));

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("c69caa40-5cd1-4ae1-9101-e84a503a79ec")) ISampleProcessor : public IUnknown
				{
					bool get_IsBypassed() = 0;
					void put_IsBypassed(bool value) = 0;

					/// <summary>
					/// Gets or sets a value indicating whether processor is suspended
					/// </summary>
					_declspec(property(get = get_IsBypassed, put = put_IsBypassed)) bool IsBypassed;

					/// <summary>
					/// Processes a sample container
					/// </summary>
					/// <param name="container">Sample container to process</param>
					/// <returns>Number of processed sample frames</returns>
					int Process(ISampleContainerPtr& container, const ProcessingContext& context) = 0;
				};

				//_COM_SMARTPTR_TYPEDEF(ISampleProcessor, __uuidof(ISampleProcessor));

			}
		}
	}
}
