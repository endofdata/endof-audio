#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleReceiver.h"
#include "IMeterChannel.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged;
using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class MeterChannel : public ISampleReceiver, public IMeterChannel
			{
			public:
				MeterChannel(int sampleRate);
				virtual ~MeterChannel();

				virtual void Flush();

				virtual void Receive(ISampleContainer& inputBuffer);

				virtual int get_RMSTime();
				virtual void put_RMSTime(int value);

				virtual float get_DbFS(int index);

				virtual MeterChannelCallback get_MeterUpdate();
				virtual void put_MeterUpdate(MeterChannelCallback value);

				virtual ISampleReceiver* get_WriteThrough();
				virtual void put_WriteThrough(ISampleReceiver* value);

				DECLARE_IUNKNOWN

			protected:
				virtual void OnMeterUpdate();

				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				MeterChannelCallback m_meterUpdate;
				ISampleReceiver* m_pWriteThrough;

				int m_sampleRate;
				int m_iSamplesPerRMSFrame;
				int m_iSumUpSamples;
				std::vector<double> m_vecSumUp;
				std::vector<float> m_vecDbFS;
			};
		}
	}
}