using NUnit.Framework;

using Audio.Foundation.Abstractions;
using Audio.Foundation.Interop;
using Audio.Foundation;

namespace Test.Audio.Foundation
{
	[TestFixture]
	public class Tests
	{
		[Test]
		public void ExportedClasses()
		{
			Level level = new(0.0d, 0.5d);
		}
	}
}