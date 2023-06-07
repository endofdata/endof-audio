#pragma once

#include <Audio.Foundation.Unmanaged.h>
#include <ISampleProcessor.h>
#include <ILooperEvents.h>
#include <comdef.h>

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_FOUNDATION_UNMANAGED_API _declspec(uuid("d446c0dc-4868-4d20-893f-0e432cf4b7ce")) ILooper : public IUnknown
				{
					bool SelectInput(int input, bool isSelected) = 0;
					bool SelectOutputPair(int outputPair[2], bool isSelected) = 0;

					void Run() = 0;

					void SaveSession(const wchar_t* pwcszFilenameBase) = 0;

					const wchar_t* AddVstPlugin(const wchar_t* pwcszVstLibrary) = 0;

					int InsertFx(const wchar_t* pwcszPluginId) = 0;
					int InsertFx(ISampleProcessorPtr effect) = 0;
					bool RemoveFx(int id) = 0;

					bool get_IsLooping() const = 0;
					_declspec(property(get = get_IsLooping)) bool IsLooping;

					int get_LoopCount() const = 0;
					_declspec(property(get = get_LoopCount)) int LoopCount;

					bool get_IsLoopRecording() const = 0;
					_declspec(property(get = get_IsLoopRecording)) bool IsLoopRecording;

					bool get_IsSessionRecording() const = 0;
					void put_IsSessionRecording(bool value) = 0;
					_declspec(property(get = get_IsSessionRecording, put = put_IsSessionRecording)) bool IsSessionRecording;

					const wchar_t* get_Name() const = 0;
					void put_Name(const wchar_t* value) = 0;
					_declspec(property(get = get_Name, put = put_Name)) const wchar_t* Name;

					ILooperEventsPtr get_LooperEvents() = 0;
					void put_LooperEvents(ILooperEventsPtr& value) = 0;
					_declspec(property(get = get_LooperEvents, put = put_LooperEvents)) ILooperEventsPtr LooperEvents;
				};

				_AUDIO_FOUNDATION_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ILooper, __uuidof(ILooper));
			}
		}
	}
}
