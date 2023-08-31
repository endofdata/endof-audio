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
		private Task? _looperTask;
		private CancellationTokenSource _tokenSource = new();
		private IAudioOutput? _selectedOutput;
		private bool _isDisposed;
		private readonly Dispatcher _dispatcher;

		/// <summary>
		/// Gets the <see cref="Looper"/> instance
		/// </summary>
		public ManagedLooper Looper
		{
			get;
		}


		public ManagedLooperStatus Status
		{
			get;
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

		public static MainModel Create(Dispatcher dispatcher, RegisteredMidiInput midiInput, RegisteredDriver driver, IEnumerable<int> inputChannels, IEnumerable<int> outputChannels)
		{
			if (driver is null)
			{
				throw new ArgumentNullException(nameof(driver));
			}

			return Create(dispatcher, midiInput.Id, driver.ClsId, inputChannels, outputChannels);
		}

		public static MainModel Create(Dispatcher dispatcher, uint midiInput, Guid guid, IEnumerable<int> inputChannels, IEnumerable<int> outputChannels)
		{
			if (inputChannels?.Any() != true)
			{
				throw new ArgumentException("No input channels selected.");
			}

			if (outputChannels?.Any() != true)
			{
				throw new ArgumentException("No output channels selected.");
			}

			var config = new ManagedLooperConfig
			{
				// with SampleCount == 0 the looper will use the device's preferred buffer size
				Name = "Lupus",
				MidiInput = midiInput,
				AsioDevice = guid,
				OutputSaturation = 0.8f
			};
			config.AddInputChannelList(inputChannels);
			config.AddOutputChannelList(outputChannels);

			return Create(dispatcher, config);
		}

		public static MainModel Create(Dispatcher dispatcher, ManagedLooperConfig config)
		{
			var looper = ManagedLooper.Create(config);

			return new MainModel(dispatcher, looper);
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="looper">A <see cref="Looper"/> that encapsules the ASIO driver access</param>
		/// <exception cref="ArgumentNullException">Thrown if <paramref name="looper"/> is <see langword="null"/></exception>
		private MainModel(Dispatcher dispatcher, ManagedLooper looper)
		{
			_dispatcher = dispatcher;

			Looper = looper ?? throw new ArgumentNullException(nameof(looper));
			Status = new ManagedLooperStatus(looper);

			//Looper.PropertyChanged += Looper_PropertyChanged;
			Looper.LoopAdded += Looper_LoopAdded;
		}

		public void RunLooper()
		{
			if (_looperTask == null)
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
			_dispatcher.InvokeAsync(() =>
			{
				Status.Tracks.Add(new ManagedLooperTrackStatus
				{
					Name = $"Loop {Status.Tracks.Count + 1}",
					Gain = 0.8f,
					Pan = 0.5f,
					ChannelCount = e.ChannelCount,
					SamplePosition = e.SamplePosition,
					SampleCount = e.SampleCount
				});
			});
		}

		protected virtual void Dispose(bool disposing)
		{
			if (!_isDisposed)
			{
				if (disposing)
				{
					StopLooper();
					Looper?.Dispose();
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
