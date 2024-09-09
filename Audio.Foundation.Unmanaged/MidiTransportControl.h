#pragma once
#include <IMidiInput.h>
#include <ITransport.h>
#include <IController.h>
#include <memory>
#include "UnknownBase.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class MidiTransportControl : public IController
			{
			public:

				MidiTransportControl(IMidiInputPtr& midiInput, ITransportPtr& transport);
				virtual ~MidiTransportControl();


				bool get_IsActive() const;
				void put_IsActive(bool value);
				_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

				void ConfigureMappings(const ControllerMapping mappings[], size_t count);
				bool GetNext(unsigned int timeout, ControllerCode& code);

				DECLARE_IUNKNOWN

			private:
				static void OnData(void* pContext, const MidiMessage& msg, unsigned int timeStamp);
				static void OnCommand(void* pContext, ControllerCode code);

				IMidiInputPtr m_input;
				ITransportPtr m_transport;
				HANDLE m_eventHandle;
				ControllerCode m_code;
				std::vector<std::pair<unsigned char, ControllerCode>> m_mappings;
				bool m_isActive;
			};
		}
	}
}