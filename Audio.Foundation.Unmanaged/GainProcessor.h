#pragma once

#include "Audio.Foundation.Unmanaged.h"
#include "UnknownBase.h"
#include "ISampleProcessor.h"
#include "ISpatial.h"
#include <vector>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class GainProcessor : public ISampleProcessor, public ISpatial
			{
			public:
				GainProcessor(double level, double pan);
				virtual ~GainProcessor();

				bool get_IsBypassed() const;
				void put_IsBypassed(bool value);

				int Process(ISampleContainerPtr& container, const ProcessingContext& context);

				double get_Level() const;
				void put_Level(double value);

				double get_Pan() const;
				void put_Pan(double value);

				DECLARE_IUNKNOWN

			protected:
				virtual bool GetInterface(REFIID riid, void** pResult);

			private:
				double m_level;
				double m_pan;
				bool m_isBypassed;
			};
		}
	}
}