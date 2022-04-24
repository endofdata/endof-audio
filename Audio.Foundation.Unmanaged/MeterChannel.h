#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleReceiver.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			typedef int(__stdcall* MeterChannelCallback)(class MeterChannel* pSender);

			class MeterChannel : public Audio::Foundation::Unmanaged::Abstractions::ISampleReceiver
			{
			public:
				MeterChannel(int sampleRate);

				void Flush();

				void Receive(Audio::Foundation::Unmanaged::Abstractions::IChannelLink& inputBuffer);

				Audio::Foundation::Unmanaged::Abstractions::IChannelLink* get_Input();
				void put_Input(Audio::Foundation::Unmanaged::Abstractions::IChannelLink* value);

				_declspec(property(get = get_RMSTime, put = put_RMSTime)) int RMSTime;

				int get_RMSTime();
				void put_RMSTime(int value);

				_declspec(property(get = get_DbLeft)) float DbLeft;

				float get_DbLeft();

				_declspec(property(get = get_DbRight)) float DbRight;

				float get_DbRight();


				_declspec(property(get = get_MeterUpdate, put = put_MeterUpdate)) MeterChannelCallback MeterUpdate;

				MeterChannelCallback get_MeterUpdate();
				void put_MeterUpdate(MeterChannelCallback value);

				DECLARE_IUNKNOWN

			protected:
				virtual void OnMeterUpdate();

				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				MeterChannelCallback m_meterUpdate;
				Audio::Foundation::Unmanaged::Abstractions::IChannelLink* m_pInput;
				int m_sampleRate;
				int m_iSamplesPerRMSFrame;
				int m_iSumUpSamples;
				double m_dSumUpLeft;
				double m_dSumUpRight;
				float m_fDbLeft;
				float m_fDbRight;
			};
		}
	}
}