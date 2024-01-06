#pragma once

#include <Audio.Asio.Unmanaged.h>
#include <ISampleProcessor.h>
#include <ILooperEvents.h>
#include <IController.h>
#include <comdef.h>

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			namespace Abstractions
			{
				__interface _AUDIO_ASIO_UNMANAGED_API _declspec(uuid("d446c0dc-4868-4d20-893f-0e432cf4b7ce")) ILooper : public IUnknown
				{
					bool SelectInput(int input, bool isSelected) = 0;
					bool SelectOutputPair(int outputPair[2], bool isSelected) = 0;

					void Run() = 0;
					void Stop() = 0;

					void SaveSession(const wchar_t* pwcszFilenameBase) = 0;

					const wchar_t* AddVstPlugin(const wchar_t* pwcszVstLibrary) = 0;

					int InsertFx(const wchar_t* pwcszPluginId) = 0;
					int InsertFx(ISampleProcessorPtr effect) = 0;
					bool RemoveFx(int id) = 0;

					bool get_IsLooping() const = 0;
					_declspec(property(get = get_IsLooping)) bool IsLooping;

					int get_LoopCount() const = 0;
					_declspec(property(get = get_LoopCount)) int LoopCount;

					RecordingStatusType get_RecordingStatus() const = 0;
					_declspec(property(get = get_RecordingStatus)) RecordingStatusType RecordingStatus;

					bool get_IsSessionRecording() const = 0;
					void put_IsSessionRecording(bool value) = 0;
					_declspec(property(get = get_IsSessionRecording, put = put_IsSessionRecording)) bool IsSessionRecording;

					const wchar_t* get_Name() const = 0;
					void put_Name(const wchar_t* value) = 0;
					_declspec(property(get = get_Name, put = put_Name)) const wchar_t* Name;

					ILooperEventsPtr get_LooperEvents() = 0;
					void put_LooperEvents(ILooperEventsPtr& value) = 0;
					_declspec(property(get = get_LooperEvents, put = put_LooperEvents)) ILooperEventsPtr LooperEvents;

					IControllerPtr get_Controller() = 0;
					void put_Controller(IControllerPtr& value) = 0;
					_declspec(property(get = get_Controller, put = put_Controller)) IControllerPtr Controller;

					MixParameter& get_LoopParameter(const GUID& id) = 0;
					_declspec(property(get = get_LoopParameter)) MixParameter LoopParameter[];

					bool RemoveLoop(const GUID& id) = 0;
				};

				_AUDIO_ASIO_UNMANAGED_API _COM_SMARTPTR_TYPEDEF(ILooper, __uuidof(ILooper));
			}
		}
	}
}
