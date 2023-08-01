using Audio.Asio.Interop;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;

namespace Lupus.Model
{
	class LooperStatus : NotifyPropertyChangedBase
    {
		private readonly Looper _looper;

        public TimeSpan Position => TimeSpan.FromMilliseconds(_looper.LoopPosition * _looper.LoopLength);

		public ObservableCollection<LooperTrackStatus> Tracks
        {
            get;
        } = new ObservableCollection<LooperTrackStatus>();

        public LooperStatus(Looper looper)
        {
            _looper = looper ?? throw new ArgumentNullException(nameof(looper));
        }
	}
}
