#pragma once

#include <AsioCore.h>
#include <ITransportControl.h>
#include <IRecorder.h>
#include <ISampleProcessor.h>
#include <ISourceJoiner.h>
#include <IVstHost.h>
#include <LooperConfig.h>
#include <UnknownBase.h>

using namespace Audio::Asio::Unmanaged;
using namespace Audio::Vst::Unmanaged::Abstractions;
using namespace Audio::Foundation::Unmanaged::Abstractions;

class Looper
{
public:
	static Looper* Create(const LooperConfig& config, const wchar_t* pwcszName);

	virtual ~Looper();

	bool SelectInput(int input, bool isSelected);
	bool SelectOutputPair(int outputPair[2], bool isSelected);

	void Run();

	void SaveSession(const wchar_t* pwcszFilenameBase);

	const wchar_t* AddVstPlugin(const wchar_t* pwcszVstLibrary);

	int InsertFx(const wchar_t* pwcszPluginId);
	int InsertFx(ISampleProcessorPtr effect);
	bool RemoveFx(int id);

	bool get_IsRecording() const;
	void put_IsRecording(bool value);
	_declspec(property(get = get_IsRecording, put = put_IsRecording)) bool IsRecording;

	const wchar_t* get_Name() const;
	void put_Name(const wchar_t* value);
	_declspec(property(get = get_Name, put = put_Name)) const wchar_t* Name;

	DECLARE_IUNKNOWN

private:
	Looper(AsioCorePtr& device);

	void CreateTransportControl(unsigned int midiInput);
	void CreateVstHost();
	void CreateProcessingChain();

	bool AddTake();

	AsioCorePtr m_device;
	IVstHostPtr m_vstHost;
	ITransportControlPtr m_transportControl;
	IRecorderPtr m_recorder;
	bool m_isRecording;
	IRecorderPtr m_masterRecorder;
	ISourceJoinerPtr m_joiner;
	int m_delay;
	std::wstring m_name;
	
};