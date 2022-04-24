#pragma once


using namespace Audio::Foundation::Templates;
using namespace Audio::Foundation::Abstractions;

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class AudioInputCollection sealed : public ReadOnlyCollection<IAudioInput^>
			{

			};
		}
	}
}
