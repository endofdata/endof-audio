#pragma once

namespace Audio
{
	namespace Asio
	{
		namespace Interop
		{
			public ref class ManagedLooperConfig sealed
			{
			public:
				ManagedLooperConfig();

				property System::String^ Name;

				property unsigned int MidiInput;

				property System::Guid AsioDevice;

				property System::Collections::Generic::IEnumerable<int>^ InputChannelList
				{
					System::Collections::Generic::IEnumerable<int>^ get();
				}

				property System::Collections::Generic::IEnumerable<int>^ OutputChannelList
				{
					System::Collections::Generic::IEnumerable<int>^ get();
				}

				property int SampleCount;

				property float OutputSaturation;

				void AddInputChannelList(System::Collections::Generic::IEnumerable<int>^ selectedChannels);

				void AddOutputChannelList(System::Collections::Generic::IEnumerable<int>^ selectedChannels);

			internal:

			private:
				System::Collections::Generic::List<int>^ _inputChannelList;
				System::Collections::Generic::List<int>^ _outputChannelList;
			};
		}
	}
}
