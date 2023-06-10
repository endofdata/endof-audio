using Lupus.Model;
using System.Windows;
using System.Windows.Input;

namespace Lupus
{
	internal partial class DeviceSelectionDialog : Window
	{
		internal DeviceSelectionModel Model
		{
			get => (DeviceSelectionModel)DataContext;
			private set => DataContext = value;
		}

		internal DeviceSelectionDialog()
		{
			InitializeComponent();
			Model = new DeviceSelectionModel();
		}

		private void CloseDialog_CanExecute(object sender, CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.SelectedDriver != null;

		private void CloseDialog_Executed(object sender, ExecutedRoutedEventArgs e) => DialogResult = true;
	}
}
