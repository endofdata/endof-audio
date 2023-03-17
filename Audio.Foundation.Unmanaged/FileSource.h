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
			class FileSource : public ISampleSource
			{
			public:
				FileSource(const std::string& filename, ISampleContainerPtr& container);
				virtual ~FileSource();

				const std::string& get_Filename() const;

				_declspec(property(get = get_Filename)) const std::string& Filename;

				int get_SampleType();

				ISampleProcessorPtr& get_First();
				void put_First(ISampleProcessorPtr& value);

				bool get_HasFirst();

				void OnNextBuffer(bool readSecondHalf);

				ISampleContainerPtr get_Container();

				DECLARE_IUNKNOWN

			protected:
				bool GetInterface(REFIID iid, void** ppvResult);

			private:
				FileReader m_reader;
				ISampleContainerPtr m_pContainer;
				ISampleProcessorPtr m_pFirst;
			};
		}
	}
}
