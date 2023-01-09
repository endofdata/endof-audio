using Audio.Asio.Interop;
using System.Collections.ObjectModel;

namespace Lupus.Model
{
	internal class DriverSelectionModel
	{
		internal DriverSelectionModel()
		{
			var registry = DriverRegistry.FromRegistry();

			foreach (var driver in registry)
			{
				RegisteredDrivers.Add(driver);
			}
		}

		public ObservableCollection<RegisteredDriver> RegisteredDrivers
		{
			get;
		} = new ObservableCollection<RegisteredDriver>();

		public RegisteredDriver? SelectedDriver
		{
			get; set;
		}
	}
}
