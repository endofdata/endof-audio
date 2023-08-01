using Audio.Asio.Interop;
using Audio.Foundation.Interop;
using Audio.Foundation.Abstractions;
using System;
using System.Linq;
using System.Collections.Generic;

namespace Lupus.Model
{
	/// <summary>
	/// Main model for Lupus
	/// </summary>
	internal class MainModel : NotifyPropertyChangedBase, IDisposable
	{
		private IAudioOutput? _selectedOutput;
		private bool _isDisposed;

		/// <summary>
		/// Gets the <see cref="Looper"/> instance
		/// </summary>
		public Looper Looper
		{
			get;
		}


		public LooperStatus Status
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

		public static MainModel Create(RegisteredMidiInput midiInput, RegisteredDriver driver, IEnumerable<int> inputChannels, IEnumerable<int> outputChannels)
		{
			if (driver is null)
			{
				throw new ArgumentNullException(nameof(driver));
			}

			return Create(midiInput.Id, driver.ClsId, inputChannels, outputChannels);
		}

		public static MainModel Create(uint midiInput, Guid guid, IEnumerable<int> inputChannels, IEnumerable<int> outputChannels)
		{
			if (inputChannels?.Any() != true)
			{
				throw new ArgumentException("No input channels selected.");
			}

			if (outputChannels?.Any() != true)
			{
				throw new ArgumentException("No output channels selected.");
			}

			var config = new LooperConfig
			{
				// with SampleCount == 0 the looper will use the device's preferred buffer size
				Name = "Lupus",
				MidiInput = midiInput,
				AsioDevice = guid,
				OutputSaturation = 0.8f
			};
			config.AddInputChannelList(inputChannels);
			config.AddOutputChannelList(outputChannels);

			return Create(config);
		}

		public static MainModel Create(LooperConfig config)
		{
			var looper = Looper.Create(config);

			return new MainModel(looper);
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="looper">A <see cref="Looper"/> that encapsules the ASIO driver access</param>
		/// <exception cref="ArgumentNullException">Thrown if <paramref name="looper"/> is <see langword="null"/></exception>
		private MainModel(Looper looper)
		{
			Looper = looper ?? throw new ArgumentNullException(nameof(looper));
			Status = new LooperStatus(looper);

			Looper.PropertyChanged += Looper_PropertyChanged;
		}

		private void Looper_PropertyChanged(object? sender, System.ComponentModel.PropertyChangedEventArgs e)
		{
			
		}

		protected virtual void Dispose(bool disposing)
		{
			if (!_isDisposed)
			{
				if (disposing)
				{
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
