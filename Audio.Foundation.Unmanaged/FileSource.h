#pragma once

#include "IInputChannel.h"
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
			class FileSource : public IInputChannel
			{
			public:
				FileSource(const std::string& filename, ISampleContainerPtr container);
				virtual ~FileSource();

				const std::string& get_Filename() const;

				_declspec(property(get = get_Filename)) const std::string& Filename;

				int get_SampleType();

				ISampleProcessorPtr get_First();
				void put_First(ISampleProcessorPtr value);

				bool get_HasFirst();

				bool get_IsActive();
				void put_IsActive(bool value);

				bool get_SupportsDirectMonitor();

				IOutputChannelPairPtr get_DirectMonitor();
				void put_DirectMonitor(IOutputChannelPairPtr value);

				bool get_HasDirectMonitor();

				void OnNextBuffer(bool readSecondHalf);

				DECLARE_IUNKNOWN

			protected:
				bool GetInterface(REFIID iid, void** ppvResult);

			private:
				bool m_isActive;
				FileReader m_reader;
				ISampleContainerPtr m_pContainer;
				ISampleProcessorPtr m_pFirst;
			};
		}
	}
}
