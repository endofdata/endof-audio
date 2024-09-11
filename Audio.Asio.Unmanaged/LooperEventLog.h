#pragma once

#include <IControllerEvents.h>
#include <ILooperEvents.h>

namespace Audio
{
	namespace Asio
	{
		namespace Unmanaged
		{
			class LooperEventLog
			{
			public:
				LooperEventLog();
				virtual ~LooperEventLog();

				void Status(const char* status);
				void Status(const wchar_t* status);
				void RecordingStatus(Audio::Asio::Unmanaged::Abstractions::RecordingStatusType status);
				void ControlCode(Audio::Foundation::Unmanaged::Abstractions::ControllerCode code);
				void Pause(bool isPaused);
				void ActivateInputChannel(int channel, bool isActive);
				void ActivateOutputChannelPair(int channelLeft, int channelRight, bool isActive);

			private:
				static void WriteMessage(const wchar_t* category, const wchar_t* message);
				static void BuildMessage(const wchar_t* category, std::function<void(std::wostringstream&)> createMessage);
			};
		}
	}
}