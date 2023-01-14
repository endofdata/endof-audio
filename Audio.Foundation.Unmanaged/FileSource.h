#pragma once

#include "ISampleSource.h"
#include "ISampleContainer.h"
#include <string>
#include "FileReader.h"
#include "UnknownBase.h"

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		namespace Unmanaged
		{
			class FileSource : public ISampleSource
			{
			public:
				FileSource(const std::string& filename, ISampleContainerPtr container);
				virtual ~FileSource();

				const std::string& get_Filename() const;

				_declspec(property(get = get_Filename)) const std::string& Filename;

				virtual bool get_IsActive();
				virtual void put_IsActive(bool value);

				virtual ISampleSharerPtr get_SampleSharer();

				virtual void Pull(bool readSecondHalf);

				virtual void Push();

				DECLARE_IUNKNOWN

			protected:
				bool GetInterface(REFIID iid, void** ppvResult);

			private:
				bool m_isActive;
				FileReader m_reader;
				ISampleContainerPtr m_container;
				ISampleSharerPtr m_sharer;
			};
		}
	}
}
