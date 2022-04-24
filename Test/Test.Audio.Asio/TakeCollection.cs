using Audio.Asio.Interop;
using System.Collections.Generic;
using System.Linq;

namespace Test.Audio.Asio
{
	internal class TakeCollection
	{
		private readonly List<(int track, IAudioTake take)> _collection = new();

		public int TrackCount => _collection.Max(t => t.track);

		public IEnumerable<IGrouping<int, IAudioTake>> Tracks => _collection.GroupBy(t => t.track, t => t.take);

		public IEnumerable<IAudioTake> this[int track] => _collection.Where(t => t.track == track).Select(t => t.take);

		public void Add(int track, IAudioTake take) => _collection.Add((track, take));
	}
}