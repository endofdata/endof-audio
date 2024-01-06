using Audio.Asio.Interop;
using Audio.Foundation.Interop;
using System.Collections.ObjectModel;

namespace Lupus.Model
{
	internal class DeviceSelectionModel
	{
		public DeviceSelectionModel()
		{
			var drivers = DriverRegistry.FromRegistry();

			foreach (var driver in drivers)
			{
				RegisteredDrivers.Add(driver);
			}

			var midiInputs = MidiInputRegistry.FromMmSystem();

			foreach (var input in midiInputs)
			{
				RegisteredMidiInputs.Add(input);
			}
		}

		public ObservableCollection<RegisteredDriver> RegisteredDrivers
		{
			get;
		} = [];

		public ObservableCollection<RegisteredMidiInput> RegisteredMidiInputs
		{
			get;
		} = [];

		public RegisteredDriver? SelectedDriver
		{
			get; set;
		}

		public RegisteredMidiInput? SelectedMidiInput
		{
			get; set;
		}
	}
}
