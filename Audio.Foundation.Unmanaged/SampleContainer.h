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
			class SampleContainer : public SampleContainerBase, public Audio::Foundation::Unmanaged::Abstractions::ISampleContainer
			{
			public:
				SampleContainer(int sampleCount);
				virtual ~SampleContainer();

				DECLARE_IUNKNOWN

				DECLARE_SAMPLECONTAINER

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);
			};
		}
	}
}
