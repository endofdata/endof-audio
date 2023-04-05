#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleProcessor.h"
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
			class MeterChannel : public IMeterChannel, public ISampleProcessor
			{
			public:
				MeterChannel(int sampleRate, int channelCount);
				virtual ~MeterChannel();

				int get_ChannelCount();

				int get_RMSTime();
				void put_RMSTime(int value);

				float get_DbFS(int index);

				MeterChannelCallback get_MeterUpdate();
				void put_MeterUpdate(MeterChannelCallback value);

				void Reset();

				bool get_IsBypassed();
				void put_IsBypassed(bool value);

				void Process(ISampleContainerPtr& inputBuffer);

				DECLARE_IUNKNOWN

			protected:
				virtual void OnMeterUpdate();

				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				static const float DbFSMin;

				MeterChannelCallback m_meterUpdate;

				int m_sampleRate;
				int m_iSamplesPerRMSFrame;
				int m_iSumUpSamples;
				std::vector<double> m_vecSumUp;
				std::vector<float> m_vecDbFS;
				bool m_isBypassed;
			};
		}
	}
}