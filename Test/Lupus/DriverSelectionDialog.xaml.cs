using Audio.Asio.Interop;
using Lupus.Model;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Lupus
{
	/// <summary>
	/// Interaction logic for DriverSelectionDialog.xaml
	/// </summary>
	internal partial class DriverSelectionDialog : Window
	{
		internal DriverSelectionModel Model
		{
			get => (DriverSelectionModel)DataContext;
			private set => DataContext = value;
		}

		internal DriverSelectionDialog()
		{
			InitializeComponent();
			Model = new DriverSelectionModel();
		}

		private void SelectDriver_CanExecute(object sender, CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.SelectedDriver != null;

		private void SelectDriver_Executed(object sender, ExecutedRoutedEventArgs e) => DialogResult = true;

		private void ListView_MouseDoubleClick(object sender, MouseButtonEventArgs e) => DialogResult = Model?.SelectedDriver != null;
	}
}
