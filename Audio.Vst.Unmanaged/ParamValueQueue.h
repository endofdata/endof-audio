#pragma once
#include <Audio.Vst.Unmanaged.h>
#include <VstCom.h>
#include <vector>

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			class ParamValueQueue : public IParamValueQueue
			{
			public:
				ParamValueQueue(const ParamID& id);
				virtual ~ParamValueQueue();

				tresult queryInterface(const TUID _iid, void** obj);

				uint32 addRef();

				uint32 release();

				ParamID getParameterId();

				int32 getPointCount();

				tresult getPoint(int32 index, int32& sampleOffset /*out*/, ParamValue& value /*out*/);

				tresult addPoint(int32 sampleOffset, ParamValue value, int32& index /*out*/);

			private:
				unsigned int m_refCount;
				ParamID m_id;
				std::vector<std::pair<int32, ParamValue>> m_points;
			};
		}
	}
}