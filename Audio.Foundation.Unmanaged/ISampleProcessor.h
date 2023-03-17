#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>
#include <ISampleContainer.h>

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
					ISampleProcessorPtr get_Next() = 0;
					void put_Next(ISampleProcessorPtr value) = 0;

					/// <summary>
					/// Gets or sets the next sample processor in the processing chain
					/// </summary>
					_declspec(property(get = get_Next, put = put_Next)) ISampleProcessorPtr Next;

					bool get_HasNext() = 0;

					/// <summary>
					/// Gets a value indicating whether a next sample processor is available
					/// </summary>
					_declspec(property(get = get_HasNext)) bool HasNext;

					/// <summary>
					/// Processes a sample container and forwards it to the next sample processor
					/// </summary>
					/// <param name="container">Sample container to process</param>
					void Process(ISampleContainerPtr& container) = 0;
				};

				//_COM_SMARTPTR_TYPEDEF(ISampleProcessor, __uuidof(ISampleProcessor));

			}
		}
	}
}
