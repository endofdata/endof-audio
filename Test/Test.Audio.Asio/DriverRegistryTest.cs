using Audio.Foundation.Abstractions;
using Audio.Foundation.Interop;
using Audio.Asio.Interop;
using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

// Use single threaded apartment for ASIO driver
[assembly: Apartment(System.Threading.ApartmentState.STA)]

namespace Test.Audio.Asio
{
	[TestFixture]
	public class DriverRegistryTest
	{
		[Test]
		public void ListDrivers()
		{
			var driverRegistry = new DriverRegistry();

			foreach (var driver in driverRegistry)
			{
				TestContext.WriteLine($"{driver.Name} {driver.ClsId}");
			}
		}

		[Test]
		public void Mooh() => RunTest(tapeMachine => PlayTake(tapeMachine, CreateMoohTake(tapeMachine)));

		[Test]
		public void Beeep() => RunTest(tapeMachine => PlayTake(tapeMachine, CreateBeeepTake(tapeMachine)));


		[Test]
		public void MultiBeep(int beeps)
		{
			RunTest(tapeMachine =>
			{
				using var memoryStream = new MemoryStream();

				var sampleRate = tapeMachine.Router.SampleRate;

				CreateTestWave(memoryStream, sampleRate, 440.0f, TimeSpan.FromSeconds(2));

				var takes = new TakeCollection();
				var offset = TimeSpan.FromSeconds(1);
				var distance = TimeSpan.FromSeconds(4);

				for (int i = 0; i < beeps; i++)
				{
					// Using the same stream for multiple audio takes does not work:
					// When the first take has played the stream is at end position. So the second
					// take has nothing to read. So, instead reuse the sample buffer, not the stream.
					var takeStream = new MemoryStream(memoryStream.ToArray(), writable: false);

					var take = new StreamAudioTake(offset, sampleRate, sampleSize: sizeof(float), takeStream, leaveOpen: false)
					{
						Volume = 0.25f
					};

					offset = offset.Add(distance);

					takes.Add(track: 1, take);
				}

				PlayTakes(tapeMachine, takes);
			});
		}

		private void RunTest(Action<TapeMachine> test)
		{
			foreach (var tapeMachine in TapeMachineProvider.MachinesByName(TapeMachineProvider.ASIO4AllDriver))
			{
				test(tapeMachine);

				var router = tapeMachine.Router;
				tapeMachine.Dispose();

				if (router != null)
				{
					router.Dispose();
				}
			}
		}

		private static void PlayTake(TapeMachine tapeMachine, IAudioTake take)
		{
			PlayTakes(tapeMachine, new IAudioTake[] { take });
		}

		private static void PlayTakes(TapeMachine tapeMachine, IEnumerable<IAudioTake> takes)
		{
			var takeCollection = new TakeCollection();

			foreach (var take in takes)
			{
				takeCollection.Add(track: 1, take: take);
			}
			PlayTakes(tapeMachine, takeCollection);
		}

		private static void PlayTakes(TapeMachine tapeMachine, TakeCollection takes)
		{
			if (tapeMachine is null)
			{
				throw new ArgumentNullException(nameof(tapeMachine));
			}

			if (takes is null)
			{
				throw new ArgumentNullException(nameof(takes));
			}

			int duration = 0;

			foreach (var takeList in takes.Tracks)
			{
				var track = tapeMachine.AddTrack();

				Assert.IsNotNull(track, $"Track ID {takeList.Key} can be added.");

				foreach (var take in takeList)
				{
					take.Initialize();

					track.InsertTake(take);

					duration = Math.Max((int)take.End.TotalMilliseconds, duration);
				}
			}

			// For debugging: save wave data from track or take data or both
			//VerifyTakeData(tapeMachine, byTrackData: true, byTakeData: true);

			bool isIdle = false;
			tapeMachine.Idle += (sender, e) => isIdle = true;

			tapeMachine.IsRunning = true;

			bool hasSignal = tapeMachine.WaitForIdle();

			Assert.That(hasSignal, Is.True, "Wait successfully for idle signal.");
			Assert.That(isIdle, Is.True, "Receive Idle event.");

			tapeMachine.IsRunning = false;

		}

		/// <summary>
		/// Writes the generated wave data from a track and from all takes to test files.
		/// </summary>
		/// <param name="tapeMachine"></param>
		/// <returns></returns>
		private static void VerifyTakeData(TapeMachine tapeMachine, bool byTrackData, bool byTakeData)
		{
			TestContext.WriteLine($"Writing test data (byTrackData: {byTrackData}, byTakeData: {byTakeData}.");

			if (!Directory.Exists("Temp"))
			{
				var folderPath = Path.GetFullPath("Temp");
				TestContext.WriteLine($"Creating output folder at '{folderPath}'.");
				Directory.CreateDirectory(folderPath);
			}

			foreach (var track in tapeMachine.Tracks)
			{
				var format = new WaveFormat
				{
					Channels = 1,
					SampleFormat = SampleFormat.IEEEFloat,
					SampleSize = sizeof(float),
					SampleCount = 0
				};

				if (byTakeData)
				{
					var takeDataPath = $"Temp\\{track.Name} TakeData.wav";

					if (File.Exists(takeDataPath))
					{
						File.Delete(takeDataPath);
					}


					using WaveFile takeData = new(takeDataPath, WaveFile.Mode.Record, format);

					int done;
					var buffer = new float[512];

					foreach (var take in track.Takes)
					{
						do
						{
							done = take.ReadNextFrame(buffer);
							takeData.WriteSamples(buffer, 0, done);

						} while (done > 0);
					}
					takeData.Close();
				}

				if (byTrackData)
				{
					var trackOutputPath = $"Temp\\{track.Name} TrackData.wav";

					if (File.Exists(trackOutputPath))
					{
						File.Delete(trackOutputPath);
					}

					tapeMachine.Position = TimeSpan.Zero;

					// Use a WaveFileOuput as the track's output, so that track data
					// is written in a wave file, when tapeMachine is running
					using var trackOutput = new WaveFileOutput(trackOutputPath, format);
					track.MonitorOut = trackOutput;

					tapeMachine.IsRunning = true;

					tapeMachine.WaitForIdle();

					tapeMachine.IsRunning = false;

					track.MonitorOut = tapeMachine.Router.Outputs[0];
					tapeMachine.Position = TimeSpan.Zero;

					trackOutput.Dispose();
				}
			}
		}

		private static IAudioTake CreateMoohTake(TapeMachine tapeMachine)
		{
			var take = new FileAudioTake(TimeSpan.FromSeconds(0), tapeMachine.Router.SampleRate, "Cow-moo-3 Mono.wav")
			{
				Volume = 0.8f
			};

			return take;
		}

		private static IAudioTake CreateBeeepTake(TapeMachine tapeMachine)
		{
			var memoryStream = new MemoryStream();
			var sampleRate = tapeMachine.Router.SampleRate;

			CreateTestWave(memoryStream, sampleRate, 440.0f, TimeSpan.FromSeconds(2));

			var take = new StreamAudioTake(TimeSpan.FromSeconds(0), sampleRate, sampleSize: sizeof(float), memoryStream, false)
			{
				Volume = 0.25f
			};

			return take;
		}

		private static void CreateTestWave(Stream stream, float rate, float frequency, TimeSpan duration)
		{
			//float periods = sampleCount / samplesPerPeriod;

			using (var writer = new BinaryWriter(stream, Encoding.Default, leaveOpen: true))
			{
				int sampleCount = (int)(rate * duration.TotalSeconds);

				float samplesPerPeriod = rate / frequency;
				float stepPerSample = (float)(Math.PI * 2.0f / samplesPerPeriod);
				float sample = 0.0f;
				float vol = 1.0f;
				int startFade = sampleCount - (int)(rate / 10);
				float fadeStep = 1.0f / (sampleCount - startFade);

				for (int pos = 0; pos < sampleCount; pos++)
				{
					if (pos >= startFade)
					{
						vol -= fadeStep;
					}
					writer.Write((float)Math.Sin(sample) * vol);

					sample += stepPerSample;
				}
				writer.Flush();
			}

			stream.Position = 0;
		}

		[OneTimeSetUp]
		public void OneTimeSetUp()
		{
			// Not required
			//COMSystem.Init(isMultiThreaded: false);
			var consoleTraceListener = new ConsoleTraceListener();
			Trace.Listeners.Add(consoleTraceListener);
			TapeMachine.TraceSource.Listeners.Add(consoleTraceListener);
			TapeMachine.TraceSource.Switch.Level = SourceLevels.All;
		}

		[OneTimeTearDown]
		public void OneTimeTearDown()
		{
			// This is required to get around RPC_E_DISCONNECTED COM error,
			// after test ran successfully.
			GC.Collect();
			GC.WaitForPendingFinalizers();
			//COMSystem.Exit();
		}
	}
}
