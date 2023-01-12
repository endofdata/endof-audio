using Audio.Foundation.Abstractions;
using Audio.Foundation.Interop;
using System;

namespace Test.Audio.Asio
{
	internal class WaveFileOutput : IAudioTarget, IDisposable
	{
		private bool _isDisposed;

		public Level DbFS
		{
			get; set;
		}

		public int ChannelId
		{
			get; set;
		}

		public WaveFile WaveFile
		{
			get;
		}

		public WaveFileOutput(string path, WaveFormat format)
		{
			WaveFile = new WaveFile(path, format);
		}

		public int Write(IAudioBuffer buffer)
		{
			return WaveFile.WriteSamples(buffer);
		}

		protected virtual void Dispose(bool disposing)
		{
			if (!_isDisposed)
			{
				if (disposing)
				{
					WaveFile.Dispose();
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
