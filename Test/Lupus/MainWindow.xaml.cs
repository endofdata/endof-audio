using Audio.Asio.Interop;
using Lupus.Controls;
using Lupus.Model;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Input;

namespace Lupus
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		//private CancellationTokenSource _cancellationTokenSource = new();
		//private Task? _meterTask;

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

		private void Run_CanExecute(object sender, CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.TapeMachine != null;

		// NFO: CanExecute checks that Model.TapeMachine is not null
		private void Run_Executed(object sender, ExecutedRoutedEventArgs e) => Model!.TapeMachine.IsRunning = ((ToggleButton)e.Source).IsChecked == true;

		private void Record_CanExecute(object sender, CanExecuteRoutedEventArgs e) => e.CanExecute = Model?.TapeMachine?.Tracks.Any(t => t.IsHot) == true;

		private void Record_Executed(object sender, ExecutedRoutedEventArgs e) => Model!.TapeMachine.IsRecording = ((ToggleButton) e.Source).IsChecked == true;


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

			// To continuously update the meter displays, collect the binding expressions of all
			// LoopTrack controls and run a background task that updates the meter values
			// TODO: Doesn't that conflict with PropertyChanged events of the lower levels?
			// At least it eats CPU tooooo much
//			var bindingExpressions = new List<BindingExpression>();

//			for (int i = 0; i < Model.Tracks.Count; i++)
//			{
//				var elementName = $"Track{i}";


//#if DEBUG
//				var loopTrack = LogicalTreeHelper.FindLogicalNode(this, elementName) as LoopTrack;

//				if (loopTrack == null)
//				{
//					break;
//				}
//#else
//				var loopTrack = LogicalTreeHelper.FindLogicalNode(this, elementName) as LoopTrack ??
//					throw new InvalidOperationException($"Unable to find '{elementName}' LoopTrack element");
//#endif

//				bindingExpressions.Add(loopTrack.GetBindingExpression(LoopTrack.DbFSProperty));
//			}

//			// Use a cancellation token to terminate on event (see Window_Closing)
//			var cancellationToken = _cancellationTokenSource.Token;

			//_meterTask = Task.Run(() =>
			//{
			//	while (!cancellationToken.IsCancellationRequested)
			//	{
			//		Task.Delay(50);

			//		Dispatcher.Invoke(() =>
			//		{
			//			foreach (var binding in bindingExpressions)
			//			{
			//				binding.UpdateTarget();
			//			}
			//		});
			//	}
			//}, cancellationToken);
		}

		private void Window_Closing(object sender, CancelEventArgs e)
		{
			//if (_meterTask != null)
			//{
			//	_cancellationTokenSource.Cancel();
			//	await _meterTask;
			//}
		}

	}
}
