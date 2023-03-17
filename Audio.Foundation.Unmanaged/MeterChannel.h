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

				virtual int get_ChannelCount();

				virtual int get_RMSTime();
				virtual void put_RMSTime(int value);

				virtual float get_DbFS(int index);

				virtual MeterChannelCallback get_MeterUpdate();
				virtual void put_MeterUpdate(MeterChannelCallback value);

				virtual void Reset();

				virtual ISampleProcessorPtr& get_next();
				virtual void put_Next(ISampleProcessorPtr &value);

				virtual bool get_HasNext();

				virtual void Process(ISampleContainerPtr& inputBuffer);

				DECLARE_IUNKNOWN

			protected:
				virtual void OnMeterUpdate();

				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				static const float DbFSMin;

				MeterChannelCallback m_meterUpdate;
				ISampleProcessorPtr m_pNext;

				int m_sampleRate;
				int m_iSamplesPerRMSFrame;
				int m_iSumUpSamples;
				std::vector<double> m_vecSumUp;
				std::vector<float> m_vecDbFS;
			};
		}
	}
}