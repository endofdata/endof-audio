using Audio.Asio.Interop;
using Audio.Foundation.Abstractions;
using System;
using System.Linq;

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
		/// Gets the <see cref="TapeMachine"/> instance
		/// </summary>
		public TapeMachine TapeMachine
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

		/// <summary>
		/// Creates a new main model using an audio device, whose name matches <paramref name="driverPattern"/>
		/// </summary>
		/// <param name="driverPattern">Name pattern for the ASIO audio driver to use as input and output device</param>
		/// <param name="maxTracks">Maximum number of looper tracks.</param>
		/// <param name="nameFormat">Format string for track names</param>
		/// <returns>Instance of <see cref="MainModel"/></returns>
		/// <exception cref="ArgumentException">Thrown if <paramref name="driverPattern"/> is <see langword="null"/> or empty or <paramref name="maxTracks"/> is less or equal to zero.</exception>
		/// <seealso cref="AsioDevice.CreateFromNameLike(string)"/>
		public static MainModel Create(string driverPattern, int maxTracks, string? nameFormat = null)
		{
			if (string.IsNullOrEmpty(driverPattern))
			{
				throw new ArgumentException($"'{nameof(driverPattern)}' cannot be null or empty.", nameof(driverPattern));
			}
			
			return Create(AsioDevice.CreateFromNameLike(driverPattern), maxTracks, nameFormat);
		}

		public static MainModel Create(RegisteredDriver driver, int maxTracks, string? nameFormat = null)
		{
			if (driver is null)
			{
				throw new ArgumentNullException(nameof(driver));
			}

			return Create(AsioDevice.CreateFromGuid(driver.ClsId), maxTracks, nameFormat);
		}

		public static MainModel Create(AsioDevice device, int maxTracks, string? nameFormat = null)
		{
			if (device is null)
			{
				throw new ArgumentNullException(nameof(device));
			}

			if (maxTracks <= 0)
			{
				throw new ArgumentException($"'{nameof(maxTracks)}' must be greater than zero.");
			}

			var inputChannels = device.AvailableInputChannels.Take(2).ToArray();

			foreach (var key in inputChannels.Select(input => input.Key))
			{
				device.SelectInputChannel(key, isSelected: true);
			}

			var outputChannels = device.AvailableOutputChannels.Take(2).ToArray();

			foreach (var key in outputChannels.Select(output => output.Key))
			{
				device.SelectOutputChannel(key, isSelected: true);
			}

			device.ActivateChannels();

			var tapeMachine = new TapeMachine(new AsioRouter(device, device));

			return new MainModel(tapeMachine, maxTracks, nameFormat);
		}

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="tapeMachine">A <see cref="TapeMachine"/> that encapsules the ASIO driver access</param>
		/// <param name="maxTracks">Maximum number of looper tracks</param>
		/// <param name="nameFormat">Format string for track names. Defaults to <c>TRK {0}</c>, where <c>{0}</c> is replaced by track ID</param>
		/// <exception cref="ArgumentNullException">Thrown if <paramref name="tapeMachine"/> is <see langword="null"/></exception>
		private MainModel(TapeMachine tapeMachine, int maxTracks, string? nameFormat = null)
		{
			TapeMachine = tapeMachine ?? throw new ArgumentNullException(nameof(tapeMachine));

			nameFormat ??= "TRK {0}";
			var defaultInput = (IAudioSource?)tapeMachine.Router.Inputs.FirstOrDefault();

			for (int i = 0; i < maxTracks; i++)
			{
				var audioTrack = tapeMachine.AddTrack();
				audioTrack.Source = defaultInput;
				audioTrack.Name = string.Format(nameFormat, i + 1);
			}
			SelectedOutput = tapeMachine.Router.Outputs.FirstOrDefault();
		}

		protected virtual void Dispose(bool disposing)
		{
			if (!_isDisposed)
			{
				if (disposing)
				{
					if (TapeMachine != null)
					{
						var router = TapeMachine.Router;
						TapeMachine.Dispose();
						router?.Dispose();
					}
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
