using Audio.Asio.Interop;
using Audio.Foundation.Interop;
using System;
using System.Linq;

namespace Lupus.Model
{
	internal class AppSettings
	{
		public DeviceSettings MidiInput
		{
			get;
		} = new();

		public DeviceSettings AsioDevice
		{
			get;
		} = new();

		public bool HasDevices => MidiInput.Name != null && AsioDevice.Name != null;

		public void UpdateDeviceSettings(DeviceSelectionModel model)
		{
			if (model.SelectedMidiInput is RegisteredMidiInput selectedMidiInput)
			{
				MidiInput.Id = selectedMidiInput.Id.ToString();
				MidiInput.Name = selectedMidiInput.Name;
			}
			else
			{
				MidiInput.Id = null;
				MidiInput.Name = null;
			}

			if (model.SelectedDriver is RegisteredDriver selectedAsioDevice)
			{
				AsioDevice.Id = selectedAsioDevice.ClsId.ToString("N");
				AsioDevice.Name = selectedAsioDevice.Name;
			}
			else
			{
				AsioDevice.Id = null;
				AsioDevice.Name = null;
			}
		}

		public DeviceSelectionModel GetDeviceSelectionModel()
		{
			var model = new DeviceSelectionModel();

			Func<RegisteredMidiInput, bool>? midiDeviceFilter = !string.IsNullOrEmpty(MidiInput.Name)?
				input => input.Name == MidiInput.Name :
				uint.TryParse(MidiInput.Id, out var id) ? 
					input => input.Id == id : null;

			model.SelectedMidiInput = midiDeviceFilter != null ? model.RegisteredMidiInputs.FirstOrDefault(midiDeviceFilter) : null;

			Func<RegisteredDriver, bool>? asioDeviceFilter = !string.IsNullOrEmpty(AsioDevice.Name) ?
				device => device.Name == AsioDevice.Name :
				Guid.TryParse(AsioDevice.Id, out var guid) ?
					device => device.ClsId == guid : null;

			model.SelectedDriver = asioDeviceFilter != null ? model.RegisteredDrivers.FirstOrDefault(asioDeviceFilter) : null;

			return model;
		}
	}
}
