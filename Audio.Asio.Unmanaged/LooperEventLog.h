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

			private:
				void WriteMessage(const wchar_t* category, const wchar_t* message);
			};
		}
	}
}