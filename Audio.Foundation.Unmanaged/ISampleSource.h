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
					ISampleProcessorPtr& get_First() = 0;
					void put_First(ISampleProcessorPtr& value) = 0;

					/// <summary>
					/// Gets or sets the first sample processor in the processing chain
					/// </summary>
					_declspec(property(get = get_First, put = put_First)) ISampleProcessorPtr First;

					bool get_HasFirst() = 0;

					/// <summary>
					/// Gets a value indicating whether a sample processor is available
					/// </summary>
					_declspec(property(get = get_HasFirst)) bool HasFirst;

					/// <summary>
					/// Processes the next audio buffer
					/// </summary>
					/// <param name="readSecondHalf">ASIO specific: is this the second half-buffer?</param>
					void OnNextBuffer(bool readSecondHalf) = 0;


					ISampleContainerPtr get_Container() = 0;

					_declspec(property(get = get_Container)) ISampleContainerPtr Container;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ISampleSource, __uuidof(ISampleSource));
			}
		}
	}
}
