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
			Model = new MainModel(Dispatcher);

			var appSettings = AppServices.GetRequiredService<IOptions<AppSettings>>().Value;

			if (appSettings.HasDevices == true)
			{
				var selectionModel = appSettings.GetDeviceSettings();
				TryCreateLooper(selectionModel);
				Model.StartLooperTask();
			}
			e.Handled = true;
		}

		private bool TryCreateLooper(DeviceSelectionModel selectionModel)
		{
			try
			{
				if (Model != null)
				{
					if (selectionModel.SelectedMidiInput is RegisteredMidiInput selectedMidiInput &&
						selectionModel.SelectedDriver is RegisteredDriver selectedDriver)
					{
						// TODO: Allow to select I/O channels
						var inputChannels = new int[] { 0, 1 };
						var outputChannels = new int[] { 0, 1 };

						Model.CreateLooper(selectedMidiInput, selectedDriver, inputChannels, outputChannels);

						return true;
					}
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

				MessageBox.Show($"Initialization failed. {builder}", Title, MessageBoxButton.OK, MessageBoxImage.Error);
			}
			return false;
		}

		private void Window_Closing(object sender, CancelEventArgs e)
		{
			Model?.Dispose();
			Model = null;
		}

		private void SoloTrack_CanExecute(object sender, System.Windows.Input.CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.Status?.SelectedTrack != null;

		private void SoloTrack_Executed(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
		{
			if (TryGetSelectedTrack(out var track))
			{
				Model!.Status!.ToggleTrackSolo(track);
			}
		}


		private void MuteTrack_CanExecute(object sender, System.Windows.Input.CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.Status?.SelectedTrack != null;

		private void MuteTrack_Executed(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
		{
			if (TryGetSelectedTrack(out var track))
			{
				Model!.Status!.ToggleTrackMute(track);
			}
		}

		private void DeleteTrack_CanExecute(object sender, System.Windows.Input.CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.Status?.SelectedTrack != null;


		private void DeleteTrack_Executed(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
		{
			if (TryGetSelectedTrack(out var track))
			{
				Model!.Looper!.RemoveLoop(track.Id);
			}
		}

		private bool TryGetSelectedTrack([NotNullWhen(true)] out ManagedLooperTrackStatus? track)
		{
			track = Model?.Status?.SelectedTrack;

			if (track == null)
			{
				MessageBox.Show("No track selected.");
			}
			return track != null;
		}

		private void Configure_CanExecute(object sender, System.Windows.Input.CanExecuteRoutedEventArgs e) => e.CanExecute = Model != null;


		private void Configure_Executed(object sender, System.Windows.Input.ExecutedRoutedEventArgs e)
		{
			if (Model == null)
			{
				throw new InvalidOperationException($"'{nameof(Model)}' cannot be null when executing command '{nameof(CustomCommands.Configure)}'.");
			}

			Model.StopLooperTask();

			var appSettings = AppServices.GetRequiredService<IOptions<AppSettings>>().Value;
			var selectionModel = appSettings.GetDeviceSettings();

			var dialog = new DeviceSelectionDialog(selectionModel)
			{
				Owner = this
			};
			if (dialog.ShowDialog() == true)
			{
				appSettings.UpdateDeviceSettings(selectionModel);
				((App)Application.Current).WriteAppSettings(appSettings);
				TryCreateLooper(selectionModel);
			}
			Model.StartLooperTask();
		}
	}
}
