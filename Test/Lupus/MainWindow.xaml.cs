﻿using Audio.Asio.Interop;
using Lupus.Model;
using System;
using System.ComponentModel;
using System.Text;
using System.Windows;
using System.Windows.Input;

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

		public MainWindow()
		{
			InitializeComponent();

		}

		private void Rewind_CanExecute(object sender, CanExecuteRoutedEventArgs e)
		{
			var position = Model?.TapeMachine?.Position;

			e.CanExecute = position != null && position != TimeSpan.Zero;
		}

		private void Rewind_Executed(object sender, ExecutedRoutedEventArgs e)
		{
			var tapeMachine = Model?.TapeMachine;

			if (tapeMachine != null)
			{
				tapeMachine.IsRunning = false;
				tapeMachine.Position = TimeSpan.Zero;
			}
		}

		private void Window_Loaded(object sender, RoutedEventArgs e)
		{
			string driverPattern = "Steinberg";
			int maxTracks = 1;

			try
			{
				Model = MainModel.Create(driverPattern, maxTracks);
			}
			catch (Exception? ex)
			{
				var builder = new StringBuilder();
				while (ex != null)
				{
					builder.AppendLine(ex.Message);
					ex = ex.InnerException;
				}
				
				MessageBox.Show($"Initialization of ASIO driver '{driverPattern}' (max. tracks: {maxTracks}) failed. {builder}", 
					Title, MessageBoxButton.OK, MessageBoxImage.Error);

				Close();
				e.Handled = true;
				return;
			}

			if (Model == null)
			{
				throw new InvalidOperationException("A model must be initialized first.");
			}

			// Configure trace source to write to console
			TapeMachine.TraceSource.Listeners.Add(new System.Diagnostics.ConsoleTraceListener());
			TapeMachine.TraceSource.Switch.Level = System.Diagnostics.SourceLevels.All;
		}

		private void Window_Closing(object sender, CancelEventArgs e)
		{
			Model?.Dispose();
			Model = null;
		}
	}
}
