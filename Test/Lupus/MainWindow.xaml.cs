using Audio.Asio.Interop;
using Audio.Foundation.Interop;
using Lupus.Model;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Options;
using System;
using System.ComponentModel;
using System.Diagnostics.CodeAnalysis;
using System.Text;
using System.Windows;

namespace Lupus
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		internal MainModel? Model
		{
			get => (MainModel?)DataContext;
			set => DataContext = value;
		}

		internal IServiceProvider AppServices => ((App)Application.Current).Services;

		public MainWindow()
		{			
			InitializeComponent();
		}

		private void Window_Loaded(object sender, RoutedEventArgs e)
		{
			RegisteredMidiInput? selectedMidiInput = null;
			RegisteredDriver? selectedDriver = null;

			try
			{
				var appSettingsMonitor = AppServices.GetRequiredService<IOptionsMonitor<AppSettings>>();				
				var appSettings = appSettingsMonitor.CurrentValue;
				var selectionModel = appSettings.GetDeviceSelectionModel();

				if (!appSettings.HasDevices)
				{
					var dialog = new DeviceSelectionDialog(selectionModel)
					{
						Owner = this
					};
					if(dialog.ShowDialog() == true)
					{
						appSettingsMonitor.OnChange(s => appSettings = s);
						((App)Application.Current).UpdateConfig(selectionModel);
					}
				}

				if(appSettings.HasDevices == true)
				{
					selectedMidiInput = selectionModel.SelectedMidiInput ?? throw new InvalidOperationException();
					selectedDriver = selectionModel.SelectedDriver ?? throw new InvalidOperationException();

					// TODO: Allow to select I/O channels
					var inputChannels = new int[] { 0, 1 };
					var outputChannels = new int[] { 0, 1 };

					Model = MainModel.Create(Dispatcher, selectedMidiInput, selectedDriver, inputChannels, outputChannels);

					Model.RunLooper();
				}
				else
				{
					Close();
					e.Handled = true;
					return;
				}
			}
			catch (Exception? ex)
			{
				var builder = new StringBuilder();
				while (ex != null)
				{
					builder.AppendLine(ex.Message);
					ex = ex.InnerException;
				}
				
				MessageBox.Show($"Initialization of looper with ASIO driver '{selectedDriver}' and MIDI input {selectedMidiInput}) failed. {builder}", 
					Title, MessageBoxButton.OK, MessageBoxImage.Error);

				Close();
				e.Handled = true;
				return;
			}
		}

		private void Window_Closing(object sender, CancelEventArgs e)
		{
			Model?.Dispose();
			Model = null;
		}

		private void SoloTrack_CanExecute(object sender, System.Windows.Input.CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.Status.SelectedTrack != null;

		private void SoloTrack_Executed(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
		{
			if (TryGetSelectedTrack(out var track))
			{
				Model!.Status.ToggleTrackSolo(track);
			}
		}


		private void MuteTrack_CanExecute(object sender, System.Windows.Input.CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.Status.SelectedTrack != null;

		private void MuteTrack_Executed(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
		{
			if (TryGetSelectedTrack(out var track))
			{
				Model!.Status.ToggleTrackMute(track);
			}
		}

		private void DeleteTrack_CanExecute(object sender, System.Windows.Input.CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.Status.SelectedTrack != null;


		private void DeleteTrack_Executed(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
		{
			if (TryGetSelectedTrack(out var track))
			{
				Model!.Looper.RemoveLoop(track.Id);
			}
		}

		private bool TryGetSelectedTrack([NotNullWhen(true)] out ManagedLooperTrackStatus? track)
		{
			track = Model?.Status.SelectedTrack;

			if (track == null)
			{
				MessageBox.Show("No track selected.");
			}
			return track != null;
		}
	}
}
