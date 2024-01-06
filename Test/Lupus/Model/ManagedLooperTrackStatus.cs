using System;

namespace Lupus.Model
{
	class ManagedLooperTrackStatus : NotifyPropertyChangedBase
    {
		private Guid _id;
		private string? _name;
		private float _pan;
		private float _gain;
		private bool _isSolo;
		private bool _isMute;

		public Guid Id
		{
			get => _id;
			set => SetValue(ref _id, value);
		}

		public string? Name
        {
            get => _name;
            set => SetValue(ref _name, value);
        }

        public float Pan
        {
            get => _pan;
            set => SetValue(ref _pan, value);
        }

        public float Gain
        {
            get => _gain;
            set => SetValue(ref _gain, value);
        }

		public bool IsSolo
		{
			get => _isSolo;
			set => SetValue(ref _isSolo, value);
		}

		public bool IsMute
		{
			get => _isMute;
			set => SetValue(ref _isMute, value);
		}

		public int ChannelCount
		{
			get;
			internal set;
		}

		public int SamplePosition
		{
			get;
			internal set;
		}

		public int SampleCount
		{
			get;
			internal set;
		}

	}
}
