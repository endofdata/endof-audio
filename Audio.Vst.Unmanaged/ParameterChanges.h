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
			class _AUDIO_VST_UNMANAGED_API ParameterChanges : public IParameterChanges
			{
			public:
				ParameterChanges();
				virtual ~ParameterChanges();

				tresult queryInterface(const TUID _iid, void** obj);

				uint32 addRef();

				uint32 release();

				int32 PLUGIN_API getParameterCount();

				IParamValueQueue* PLUGIN_API getParameterData(int32 index);

				IParamValueQueue* PLUGIN_API addParameterData(const ParamID& id, int32& index /*out*/);

			private:
				unsigned int m_refCount;
				std::vector<IParamValueQueuePtr> m_parameters;
			};
		}
	}
}