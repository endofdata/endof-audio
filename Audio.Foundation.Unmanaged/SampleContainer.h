#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleContainer.h"
#include "ISampleBuffer.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleContainer abstract : public UnknownBase, public Audio::Foundation::Unmanaged::Abstractions::ISampleContainer
			{
			public:
				SampleContainer(int sampleCount);
				virtual ~SampleContainer();

				virtual Abstractions::ISampleBuffer* get_LeftChannel();
				virtual Abstractions::ISampleBuffer* get_RightChannel();

				virtual bool get_IsActive();
				virtual void put_IsActive(bool value);

				_declspec(property(get = get_IsActive, put = put_IsActive)) bool IsActive;

				virtual int get_SampleCount();
				virtual void put_SampleCount(int sampleCount);

				_declspec(property(get = get_SampleCount, put = put_SampleCount)) int SampleCount;

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				void AllocChannels(int sampleCount);
				void FreeChannels();

				Abstractions::ISampleBuffer* m_pLeftChannel;
				Abstractions::ISampleBuffer* m_pRightChannel;
				int m_sampleCount;
				bool m_isActive;
			};
		}
	}
}
