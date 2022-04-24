#pragma once

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class AudioOutputCollection sealed : public Audio::Foundation::Templates::ReadOnlyCollection<Audio::Foundation::Abstractions::IAudioOutput^>
			{
			};
		}
	}
}
