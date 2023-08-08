using Audio.Asio.Interop;
using System;
using System.Collections.ObjectModel;

namespace Lupus.Model
{
	class ManagedLooperStatus : NotifyPropertyChangedBase
    {
		private readonly ManagedLooper _looper;

        public TimeSpan Position => TimeSpan.FromMilliseconds(_looper.LoopPosition * _looper.LoopLength);

		public ObservableCollection<ManagedLooperTrackStatus> Tracks
        {
            get;
        } = new ObservableCollection<ManagedLooperTrackStatus>();

        public ManagedLooperStatus(ManagedLooper looper)
        {
            _looper = looper ?? throw new ArgumentNullException(nameof(looper));
        }
	}
}
