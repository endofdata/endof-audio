#include "pch.h"
#include "LooperEventLog.h"

using namespace Audio::Asio::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;
using namespace Audio::Asio::Unmanaged::Abstractions;

LooperEventLog::LooperEventLog()
{
}

LooperEventLog::~LooperEventLog()
{
}

void LooperEventLog::Status(const char* status)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	//std::string narrow = converter.to_bytes(wide_utf16_source_string);
	std::wstring wide = converter.from_bytes(status);

	Status(wide.c_str());
}

void LooperEventLog::Status(const wchar_t* status)
{
	WriteMessage(L"LooperStatus", status);	
}

void LooperEventLog::RecordingStatus(RecordingStatusType status)
{
	const wchar_t* statusText;

	switch (status)
	{
	case RecordingStatusType::Off:
		statusText = L"Off";
		break;
	case RecordingStatusType::Armed:
		statusText = L"Armed";
		break;
	case RecordingStatusType::Recording:
		statusText = L"Recording";
		break;
	case RecordingStatusType::Unarmed:
		statusText = L"Unarmed";
		break;
	default:
		statusText = L"<invalid>";
		break;
	}
	WriteMessage(L"LooperRecording", statusText);
}

void LooperEventLog::ControlCode(ControllerCode code)
{
	const wchar_t* codeText;

	switch (code)
	{
	case ControllerCode::Run:
		codeText = L"Run";
		break;
	case ControllerCode::Stop:
		codeText = L"Stop";
		break;
	case ControllerCode::Record:
		codeText = L"Record";
		break;
	case ControllerCode::Cancel:
		codeText = L"Cancel";
		break;
	case ControllerCode::Locate:
		codeText = L"Locate";
		break;
	case ControllerCode::None:
		codeText = L"None";
		break;
	default:
		codeText = L"<unknown>";
		break;
	}
	WriteMessage(L"LooperControl", codeText);
}

void LooperEventLog::WriteMessage(const wchar_t* category, const wchar_t* message)
{
	std::wostringstream builder;

	builder << L"[" << category << L"] " << message << std::endl;

	OutputDebugStringW(builder.str().c_str());
}
