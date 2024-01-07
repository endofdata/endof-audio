using Audio.Asio.Interop;
using Audio.Foundation.Interop;
using Audio.Foundation.Abstractions;
using System;
using System.Linq;
using System.Collections.Generic;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Threading;

namespace Lupus.Model
{
	/// <summary>
	/// Main model for Lupus
	/// </summary>
	internal class MainModel : NotifyPropertyChangedBase, IDisposable
	{
		private ManagedLooper? _looper;
		private Task? _looperTask;
		private CancellationTokenSource _tokenSource = new();
		private IAudioOutput? _selectedOutput;
		private bool _isDisposed;
		private readonly Dispatcher _dispatcher;

		/// <summary>
		/// Gets the <see cref="Looper"/> instance
		/// </summary>
		public ManagedLooper? Looper
		{
			get => _looper;

			private set
			{
				if (value != _looper)
				{
					if (_looper != null)
					{
						_looper.Stop();
						_looper.PropertyChanged -= Looper_PropertyChanged;
						_looper.LoopAdded -= Looper_LoopAdded;
						Status = null;
						_looper.Dispose();
						_looper = null;
					}

					_looper = value;

					if (_looper != null)
					{
						Status = new ManagedLooperStatus(_looper);

						//_looper.PropertyChanged += Looper_PropertyChanged;
						_looper.LoopAdded += Looper_LoopAdded;
					}
				}

			}
		}

		public string? DriverName
		{
			get; private set;
		}

		public string? MidiInputName
		{
			get; private set;
		}

		public ManagedLooperStatus? Status
		{
			get; private set;
		}


		/// <summary>
		/// Gets or sets the current output
		/// </summary>
		/// <remarks>
		/// Changing the current input calls <see cref="LoopTrackModel.UpdateMonitorOut(IAudioOutput?, IAudioOutput?)"/> for all tracks.
		/// </remarks>
		public IAudioOutput? SelectedOutput
		{
			get => _selectedOutput;
			set => SetValue(ref _selectedOutput, value);
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <exception cref="ArgumentNullException">Thrown if <paramref name="looper"/> is <see langword="null"/></exception>
		public MainModel(Dispatcher dispatcher)
		{
			_dispatcher = dispatcher ?? throw new ArgumentNullException(nameof(dispatcher));
		}

		public void CreateLooper(RegisteredMidiInput midiInput, RegisteredDriver driver, IEnumerable<int> inputChannels, IEnumerable<int> outputChannels)
		{
			if (driver is null)
			{
				throw new ArgumentNullException(nameof(driver));
			}

			if (inputChannels?.Any() != true)
			{
				throw new ArgumentException("No input channels selected.");
			}

			if (outputChannels?.Any() != true)
			{
				throw new ArgumentException("No output channels selected.");
			}

			Looper = null;

			var config = new ManagedLooperConfig
			{
				// with SampleCount == 0 the looper will use the device's preferred buffer size
				Name = "Lupus",
				MidiInput = midiInput.Id,
				AsioDevice = driver.ClsId,
				OutputSaturation = 0.8f
			};
			config.AddInputChannelList(inputChannels);
			config.AddOutputChannelList(outputChannels);

			Looper = ManagedLooper.Create(config);

			DriverName = driver.Name;
			MidiInputName = midiInput.Name;
		}

		public void RunLooper()
		{
			if (_looperTask == null && Looper != null)
			{
				_looperTask = Looper.RunAsync(_tokenSource.Token);
			}
		}

		public void StopLooper()
		{
			if (_looperTask != null)
			{
				_tokenSource.Cancel();
				_looperTask.GetAwaiter().GetResult();
				_looperTask = null;
			}
		}

		private void Looper_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
		{
			bool goo = false;

			if (e.PropertyName == nameof(Looper.TransportPosition))
			{
				goo = true;
			}
		}

		private void Looper_LoopAdded(object? sender, LoopEventArgs e)
		{
			if (Status == null)
			{
				throw new InvalidOperationException("Looper intiialization incomplete");
			}
			_dispatcher.InvokeAsync(() =>
			{
				var track = new ManagedLooperTrackStatus
				{
					Id = e.Id,
					Name = $"Loop {Status.GetNextLoopNumber()}",
					Gain = 0.8f,
					Pan = 0.5f,
					ChannelCount = e.ChannelCount,
					SamplePosition = e.SamplePosition,
					SampleCount = e.SampleCount
				};
				Status.Tracks.Add(track);
				Status.SelectedTrack = track;
			});
		}

		protected virtual void Dispose(bool disposing)
		{
			if (!_isDisposed)
			{
				if (disposing)
				{
					// Stops and disposes current looper, if any
					Looper = null;
				}
				_isDisposed = true;
			}
		}

		public void Dispose()
		{
			// Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
			Dispose(disposing: true);
			GC.SuppressFinalize(this);
		}
	}
}
