using Lupus.Model;
using System;
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

		internal DeviceSelectionDialog(DeviceSelectionModel model)
		{
			if (model is null)
			{
				throw new ArgumentNullException(nameof(model));
			}

			InitializeComponent();
			Model = model;
		}

		private void CloseDialog_CanExecute(object sender, CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.SelectedDriver != null;

		private void CloseDialog_Executed(object sender, ExecutedRoutedEventArgs e) => DialogResult = true;
	}
}
