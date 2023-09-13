#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <comdef.h>

using namespace Audio::Foundation::Unmanaged;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				typedef int (*RequestDataHandler)(void* pContext, const char* pRequest, int size, char* pResponse, int maxResponse);
				typedef int (*ContinueResponseHandler)(void* pContext, char* pResponse, int maxResponse);

				__interface _AUDIO_FOUNDATION_UNMANAGED_API __declspec(uuid("815005d3-736d-402d-b8f9-a4d85258b048")) IServerEvents : public IUnknown
				{
					void* get_Context() const = 0;
					void put_Context(void* value) = 0;
					/// <summary>
					/// Gets or sets the context parameter for the transport event handler
					/// </summary>
					_declspec(property(get = get_Context, put = put_Context)) void* Context;


					RequestDataHandler get_OnRequestData() const = 0;
					void put_OnRequestData(RequestDataHandler value) = 0;
					/// <summary>
					/// Gets or sets the handler for request data
					/// </summary>
					_declspec(property(get = get_OnRequestData, put = put_OnRequestData)) RequestDataHandler OnRequestData;

					ContinueResponseHandler get_OnContinueResponse() const = 0;
					void put_OnContinueResponse(ContinueResponseHandler value) = 0;
					/// <summary>
					/// Gets or sets the handler for additional response data
					/// </summary>
					_declspec(property(get = get_OnContinueResponse, put = put_OnContinueResponse)) ContinueResponseHandler OnContinueResponse;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(IServerEvents, __uuidof(IServerEvents));
			}
		}
	}
}