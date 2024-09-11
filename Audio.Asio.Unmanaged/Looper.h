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
#include <LooperEventLog.h>
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

				bool SelectInput(int inputIdx, bool isSelected);
				bool SelectOutputPair(int outputPairIdx, bool isSelected);

				void Start();
				bool Stop(DWORD waitTimeout);
				bool Wait(DWORD waitTimeout);

				void SaveSession(const wchar_t* pwcszFilenameBase);

				const wchar_t* AddVstPlugin(const wchar_t* pwcszVstLibrary);

				int InsertFx(const wchar_t* pwcszPluginId);
				int InsertFx(ISampleProcessorPtr effect);
				bool RemoveFx(int id);

				bool get_IsRunning() const;

				bool get_IsLooping() const;

				int get_LoopCount() const;

				Audio::Foundation::Unmanaged::AudioTime get_LoopLength() const;

				Audio::Foundation::Unmanaged::AudioTime get_TransportPosition() const;
				void put_TransportPosition(Audio::Foundation::Unmanaged::AudioTime value);

				RecordingStatusType get_RecordingStatus() const;

				bool get_IsSessionRecording() const;
				void put_IsSessionRecording(bool value);

				bool get_IsPaused() const;
				void put_IsPaused(bool value);

				const wchar_t* get_Name() const;
				void put_Name(const wchar_t* value);

				ILooperEventsPtr get_LooperEvents();
				void put_LooperEvents(ILooperEventsPtr& value);

				IControllerPtr get_Controller();
				void put_Controller(IControllerPtr& value);

				MixParameter& get_LoopParameter(const GUID& id);

				bool RemoveLoop(const GUID& id);

				DECLARE_IUNKNOWN

			private:
				Looper(AsioCorePtr& device);

				void CreateController(unsigned int midiInput);
				void CreateVstHost();
				void CreateProcessingChain();

				void Run();
				void Wrap();
				void ArmRecording();
				void UnarmRecording();
				void StartRecording();
				void StopRecording();
				bool DropRecording();
				bool PausePlayback(ITransportPtr& transport);
				bool AddLoop(int maxSamples);

				void OnStarting();
				void OnStopping();
				void OnHeartbeat(ITransportPtr& transport);
				void OnRecordingStatusChanged();
				void OnIsLoopingChanged();
				void OnIsSessionRecordingChanged();
				void OnLoopRestart();
				void OnAddLoop(const GUID& id, int channelCount, int samplePosition, int sampleCount);
				void OnRemoveLoop(const GUID& id);
				void OnDropRecording();
				void OnIsPaused(bool isPaused);

				static DWORD ControlThreadEntry(LPVOID param);
				void ControlThreadExit();

				HANDLE m_controlThread;
				DWORD m_controlThreadId;

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
				LooperEventLog m_eventLog;

				int m_delay;
				bool m_stopCalled;
				std::wstring m_name;
			};
		}
	}
}