using Audio.Asio.Interop;
using System;
using System.Collections.ObjectModel;

namespace Lupus.Model
{
	class ManagedLooperStatus : NotifyPropertyChangedBase
	{
		private readonly ManagedLooper _looper;
		private int _nextLoopNumber;
		private ManagedLooperTrackStatus? _selectedTrack;
		private Collection<ManagedLooperTrackStatus> _soloTracks = new();

		public TimeSpan Position => TimeSpan.FromMilliseconds(_looper.LoopPosition * _looper.LoopLength);

		public ManagedLooperTrackStatus? SelectedTrack
		{
			get => _selectedTrack;
			set => SetValue(ref _selectedTrack, value);
		}

		public ObservableCollection<ManagedLooperTrackStatus> Tracks
		{
			get;
		} = [];

		public ManagedLooperStatus(ManagedLooper looper)
		{
			_looper = looper ?? throw new ArgumentNullException(nameof(looper));
		}

		public int GetNextLoopNumber() => ++_nextLoopNumber;

		public void ToggleTrackSolo(ManagedLooperTrackStatus track)
		{
			if (track is null)
			{
				throw new ArgumentNullException(nameof(track));
			}

			track.IsSolo = !track.IsSolo;

			if (track.IsSolo)
			{
				if (_soloTracks.Count == 0)
				{
					foreach (var item in Tracks)
					{
						if (item.Id != track.Id)
						{
							_looper.GetTrack(item.Id).Level = 0.0;
						}
					}
				}

				_soloTracks.Add(track);
				_looper.GetTrack(track.Id).Level = track.Gain;
			}
			else
			{
				_soloTracks.Remove(track);

				if (_soloTracks.Count == 0)
				{
					foreach (var item in Tracks)
					{
						if (item.Id != track.Id)
						{
							_looper.GetTrack(item.Id).Level = item.Gain;
						}
					}
				}
				else
				{
					_looper.GetTrack(track.Id).Level = 0.0;
				}
			}
		}

		public void ToggleTrackMute(ManagedLooperTrackStatus track)
		{
			if (track is null)
			{
				throw new ArgumentNullException(nameof(track));
			}

			track.IsMute = !track.IsMute;
			_looper.GetTrack(track.Id).Level = track.IsMute ? 0.0 : track.Gain;
		}
	}
}
