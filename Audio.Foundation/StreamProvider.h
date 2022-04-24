#pragma once

#include <SampleContainer.h>
#include <SampleSharer.h>
#include <vcclr.h>

using namespace Audio::Foundation::Unmanaged::Abstractions;

namespace Audio
{
	namespace Foundation
	{
		public class StreamProvider
		{
		public:
			StreamProvider(int sampleCount, System::IO::Stream^ fred);
			virtual ~StreamProvider();

			ISampleContainer& get_Container();

			_declspec(property(get = get_Container)) ISampleContainer& Container;

			ISampleSharer& get_Sharer();

			_declspec(property(get = get_Sharer)) ISampleSharer& Sharer;

		private:
			gcroot<System::IO::Stream^> m_stream;

			ISampleContainer* m_pContainer;
			ISampleSharer* m_pSharer;
		};
	}
}