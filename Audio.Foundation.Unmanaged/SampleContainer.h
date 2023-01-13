#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleContainer.h"
#include "SampleContainerBase.h"

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class SampleContainer : public SampleContainerBase
			{
			public:
				SampleContainer(int sampleCount, int channelCount);
				virtual ~SampleContainer();

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);
			};
		}
	}
}
