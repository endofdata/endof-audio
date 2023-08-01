﻿using Audio.Asio.Interop;
using Audio.Foundation.Interop;
using Lupus.Model;
using System;
using System.ComponentModel;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;

namespace Lupus
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		private Task? _looperTask;
		private CancellationTokenSource _tokenSource = new();

		internal MainModel? Model
		{
			get => (MainModel?)DataContext;
			set => DataContext = value;
		}

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
				var dialog = new DeviceSelectionDialog
				{
					Owner = this					
				};

				if (dialog.ShowDialog() == true)
				{
					selectedMidiInput = dialog.Model.SelectedMidiInput ?? throw new InvalidOperationException();
					selectedDriver = dialog.Model.SelectedDriver ?? throw new InvalidOperationException();

					// TODO: Allow to select I/O channels
					var inputChannels = new int[] { 0, 1 };
					var outputChannels = new int[] { 0, 1 };

					Model = MainModel.Create(selectedMidiInput, selectedDriver, inputChannels, outputChannels);

					var looper = Model.Looper;

					_looperTask = Task.Run(() => looper.Run(), _tokenSource.Token);
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
			if (_looperTask != null)
			{
				_tokenSource.Cancel();
				_looperTask.GetAwaiter().GetResult();
			}
			Model?.Dispose();
			Model = null;
		}
    }
}
