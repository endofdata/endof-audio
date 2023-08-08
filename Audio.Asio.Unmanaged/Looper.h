#pragma once

#include <AsioCore.h>
#include <IController.h>
#include <IRecorder.h>
#include <ILooperConfig.h>
#include <ISampleProcessor.h>
#include <ISourceJoiner.h>
#include <IVstHost.h>
#include <ILooper.h>
#include <ILooperEvents.h>
#include <LooperConfig.h>
#include <UnknownBase.h>

using namespace Audio::Asio::Unmanaged;
using namespace Audio::Vst::Unmanaged::Abstractions;
using namespace Audio::Asio::Unmanaged::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			class Looper : public ILooper
			{
			public:
				static Looper* Create(const ILooperConfig& config);

				virtual ~Looper();

				bool SelectInput(int input, bool isSelected);
				bool SelectOutputPair(int outputPair[2], bool isSelected);

				void Run();
				void Stop();

				void SaveSession(const wchar_t* pwcszFilenameBase);

				const wchar_t* AddVstPlugin(const wchar_t* pwcszVstLibrary);

				int InsertFx(const wchar_t* pwcszPluginId);
				int InsertFx(ISampleProcessorPtr effect);
				bool RemoveFx(int id);

				bool get_IsLooping() const;

				int get_LoopCount() const;

				RecordingStatusType get_RecordingStatus() const;

				bool get_IsSessionRecording() const;
				void put_IsSessionRecording(bool value);

				const wchar_t* get_Name() const;
				void put_Name(const wchar_t* value);

				ILooperEventsPtr get_LooperEvents();
				void put_LooperEvents(ILooperEventsPtr& value);

				IControllerPtr get_Controller();
				void put_Controller(IControllerPtr& value);

				DECLARE_IUNKNOWN

			private:
				Looper(AsioCorePtr& device);

				void CreateController(unsigned int midiInput);
				void CreateVstHost();
				void CreateProcessingChain();

				void ArmRecording();
				void UnarmRecording();
				void StartRecording();
				void StopRecording();
				bool DropRecording();
				bool AddLoop();

				void OnHeartbeat(ITransportPtr& transport);
				void OnRecordingStatusChanged();
				void OnIsLoopingChanged();
				void OnIsSessionRecordingChanged();
				void OnLoopRestart();
				void OnAddLoop();
				void OnDropRecording();

				AsioCorePtr m_device;
				IVstHostPtr m_vstHost;
				IControllerPtr m_controller;
				ILooperEventsPtr m_events;
				IRecorderPtr m_recorder;
				RecordingStatusType m_recordingStatus;
				ProcessingContext* m_context;
				bool m_isSessionRecording;
				IRecorderPtr m_sessionRecorder;
				ISourceJoinerPtr m_joiner;
				int m_delay;
				bool m_stopCalled;
				std::wstring m_name;

			};
		}
	}
}