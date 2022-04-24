#pragma once


namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			ref class AudioTrack;

			public ref class AudioTrackCollection sealed : public Audio::Foundation::Templates::ReadOnlyCollection<AudioTrack^>
			{
			public:
				AudioTrack^ Find(int trackId);
			};
		}
	}
}
