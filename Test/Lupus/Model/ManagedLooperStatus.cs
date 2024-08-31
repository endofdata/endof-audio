using Audio.Asio.Interop;
using System;
using System.Collections.ObjectModel;

namespace Lupus.Model
{
	class ManagedLooperStatus(ManagedLooper looper) : NotifyPropertyChangedBase
	{
		private readonly ManagedLooper _looper = looper ?? throw new ArgumentNullException(nameof(looper));
		private int _nextLoopNumber;
		private ManagedLooperTrackStatus? _selectedTrack;
		private readonly Collection<ManagedLooperTrackStatus> _soloTracks = [];

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

		public int GetNextLoopNumber() => ++_nextLoopNumber;

		public void ToggleTrackSolo(ManagedLooperTrackStatus track)
		{
			ArgumentNullException.ThrowIfNull(track);

			track.IsSolo = !track.IsSolo;

			if (track.IsSolo)
			{
				if (_soloTracks.Count == 0)
				{
					foreach (var item in Tracks)
					{
						if (item.Id != track.Id)
						{
							WithTrack(item.Id, t => t.Level = 0.0);
						}
					}
				}

				_soloTracks.Add(track);
				WithTrack(track.Id, t => t.Level = track.Gain);
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
							WithTrack(item.Id, t => t.Level = item.Gain);
						}
					}
				}
				else
				{
					WithTrack(track.Id, t=> t.Level = 0.0);
				}
			}
		}

		public void ToggleTrackMute(ManagedLooperTrackStatus track)
		{
			ArgumentNullException.ThrowIfNull(track);

			track.IsMute = !track.IsMute;
			WithTrack(track.Id, t => t.Level = track.IsMute ? 0.0 : track.Gain);
		}

		private ManagedLooperTrack? GetTrack(Guid id) => id == Guid.Empty ? null : _looper.GetTrack(id);

		private void WithTrack(Guid id, Action<ManagedLooperTrack> action)
		{
			if (GetTrack(id) is ManagedLooperTrack track)
			{
				action(track);
			}
		}
	}
}
