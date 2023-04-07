#pragma once
#include <IMidiInput.h>
#include <ITransport.h>
#include <memory>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace RepeatMyself
{
	typedef std::shared_ptr<class TransportControl> TransportControlPtr;

	class TransportControl
	{
	public:

		TransportControl(IMidiInputPtr& midiInput, ITransportPtr& transport);
		virtual ~TransportControl();

		bool get_IsActive() const;
		void put_IsActive(bool value);
		_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

		bool GetNext(unsigned int timeout, TransportCode& code);

		static TransportControlPtr Create(ITransportPtr& transport);

	private:
		static void OnData(void* pContext, const MidiMessage& msg, unsigned int timeStamp);
		static void OnTransport(void* pContext, TransportCode code);

		IMidiInputPtr m_input;
		ITransportPtr m_transport;
		HANDLE m_eventHandle;
		TransportCode m_code;
		bool m_isActive;
	};

	
}
