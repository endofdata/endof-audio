#pragma once
#include <IMidiInput.h>
#include <memory>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace RepeatMyself
{
	enum class TransportCode
	{
		None,
		Stop,
		Play,
		Record,
		ToStart,
		ToEnd
	};

	typedef std::shared_ptr<class TransportControl> TransportControlPtr;

	class TransportControl
	{
	public:

		TransportControl(IMidiInputPtr& midiInput);
		virtual ~TransportControl();

		bool get_IsActive() const;
		void put_IsActive(bool value);
		_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

		bool GetNext(unsigned int timeout, TransportCode& code);

		static TransportControlPtr Create();

	private:
		static void OnData(void* pContext, const MidiMessage& msg, unsigned int timeStamp);

		IMidiInputPtr m_input;
		HANDLE m_eventHandle;
		TransportCode m_code;
		bool m_isActive;
	};

	
}
