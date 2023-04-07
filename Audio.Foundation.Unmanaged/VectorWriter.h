#pragma once

#include "ISampleProcessor.h"
#include "IRecorder.h"
#include "UnknownBase.h"
#include <vector>
#include <mutex>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class VectorWriter : public IRecorder, public ISampleProcessor
			{
			public:
				VectorWriter(int channelCount = 0, int initialSize = 0, int growth = 0);
				virtual ~VectorWriter();

				int Process(ISampleContainerPtr& container);

				bool get_IsBypassed();
				void put_IsBypassed(bool value);

				ISampleContainerPtr CreateSampleContainer(bool continueRecording);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);
				void InitializeBuffers();

			private:
				int m_channelCount;
				int m_initial;
				int m_growth;
				int m_inUse;
				int m_avail;
				std::vector<Sample*> m_buffers;
				std::mutex m_buffers_mutex;
				bool m_isBypassed;
			};
		}
	}
}