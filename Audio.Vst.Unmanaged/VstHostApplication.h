#pragma once
#include <Audio.Vst.Unmanaged.h>
#include <ivsthostapplication.h>
#include <memory>
#include <vector>
#include "PluginLibrary.h"

using namespace Steinberg;
using namespace Steinberg::Vst;

namespace Audio
{
	namespace Vst
	{
		namespace Unmanaged
		{
			class VstHostApplication : public IHostApplication
			{
			public:
				VstHostApplication(const std::wstring& name);
				virtual ~VstHostApplication();

				tresult queryInterface(const TUID _iid, void** obj);

				uint32 addRef();

				uint32 release();

				tresult getName(String128 name);

				tresult createInstance(TUID cid, TUID _iid, void** obj);

			private:
				unsigned int m_refCount;
				std::wstring m_name;
			};
		}
	}
}