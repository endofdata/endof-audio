using Audio.Asio.Interop;
using Audio.Foundation.Interop;
using NUnit.Framework;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace Test.Audio.Asio
{
	public class DeviceAllocation
	{
		[Test]
		public void ReallocDevice()
		{
			var drivers = DriverRegistry.FromRegistry();

			var driver = drivers.First(d => d.Name.Contains("Steinberg"));

			TestContext.WriteLine("Use device once.");
			Assert.That(() => UseDevice(driver), Throws.Nothing, "Can acquire device once.");

			TestContext.WriteLine("Use device a second time.");
			Assert.That(() => UseDevice(driver), Throws.Nothing, "Can acquire device twice.");
		}

		private void UseDevice(RegisteredDriver driver)
		{
			AsioDevice? maybeDevice = null;

			Assert.That(() => maybeDevice = AsioDevice.CreateFromGuid(driver.ClsId), Throws.Nothing, $"Can acquire device '{driver.Name}' once.");
			Assert.That(maybeDevice, Is.Not.Null, "Acquired device instance is not null.");

			var device = maybeDevice!;

			device.SelectInputChannel(0, true);
			device.SelectOutputChannel(0, true);
			device.SelectOutputChannel(1, true);
			device.ActivateChannels();

			device.IsPoweredOn = true;

			Thread.Sleep(1000);

			//device.IsPoweredOn = false;

			Assert.That(() => device.Dispose(), Throws.Nothing, "Can dispose device instance.");
		}

		[Test]
		public void ReallocManagedLooper()
		{
			var drivers = DriverRegistry.FromRegistry();
			var driver = drivers.First(d => d.Name.Contains("Steinberg"));
			var inputs = MidiInputRegistry.FromMmSystem();
			var input = inputs.First(i => i.Id == 1);

			var config = new ManagedLooperConfig
			{
				AsioDevice = driver.ClsId,
				MidiInput = input.Id,
				OutputSaturation = 0.75f,
			};
			config.AddInputChannelList(new int[] { 0, 1 });
			config.AddOutputChannelList(new int[] { 0, 1 });

			TestContext.WriteLine("Use looper once.");
			UseLooper(config);

			TestContext.WriteLine("Use looper a second time.");
			UseLooper(config);
		}

		private async void UseLooper(ManagedLooperConfig config)
		{
			ManagedLooper? maybeLooper = null;

			TestContext.WriteLine("Create managed looper.");
			Assert.That(() => maybeLooper = ManagedLooper.Create(config), Throws.Nothing, "Can create looper.");
			Assert.That(maybeLooper, Is.Not.Null, "Managed looper instance is not null.");

			var looper = maybeLooper!;
			using var tokenSource = new CancellationTokenSource();

			TestContext.WriteLine("Starting looper task for one second.");
			Task? maybeLooperTask = looper.RunAsync(tokenSource.Token);
			Assert.That(maybeLooperTask, Is.Not.Null, "Looper task instance is not null.");

			tokenSource.CancelAfter(1000);
			var looperTask = maybeLooperTask!;

			try
			{
				TestContext.WriteLine("Awaiting looper task.");
				await looperTask.ConfigureAwait(false);
			}
			catch (TaskCanceledException)
			{
				TestContext.WriteLine("Task cancelled.");
			}
			Assert.That(() => looper.Dispose(), Throws.Nothing, "Can dispose looper.");
		}
	}
}
