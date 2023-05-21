using Audio.Foundation.Interop;
using NUnit.Framework;
using System;
using System.IO;

namespace Test.Audio.Foundation
{
	[TestFixture]
	public class WaveFileTests
	{
		[Test]
		public void WaveFileIO()
		{
			var name = Path.Combine(Path.GetTempPath(), $"_{Guid.NewGuid():D}.wav");

			try
			{
				var format = new WaveFormat
				{
					Channels = 1,
					SampleFormat = SampleFormat.IEEEFloat,
					SampleRate = 48000,
					SampleSize = sizeof(float)
				};

				var file = new WaveFile(name, format);
				var data = new float[1024];

				try
				{
					var sampleMin = -1.0f;
					var sampleMax = 1.0f;

					var step = (float)(sampleMax - sampleMin) / data.Length;
					var sample = sampleMin;

					for (int i = 0; i < data.Length; i++)
					{
						data[i] = sample;
						sample += step;
					}

					file.WriteSamples(data, 0, data.Length);
				}
				finally
				{
					file.Close();
				}

				var resultFile = new WaveFile(name);

				try
				{
					var resultFormat = resultFile.Format;

					Assert.That(resultFormat.Channels, Is.EqualTo(format.Channels), "Channel count is OK.");
					Assert.That(resultFormat.SampleFormat, Is.EqualTo(format.SampleFormat), "SampleFormat is OK.");
					Assert.That(resultFormat.SampleRate, Is.EqualTo(format.SampleRate), "SampleRate is OK.");
					Assert.That(resultFormat.SampleSize, Is.EqualTo(format.SampleSize), "SampleSize is OK.");
					Assert.That(resultFormat.SampleCount, Is.EqualTo(data.Length), "Sample count is OK");

					resultFile.MoveToSamples();

					var resultData = new float[resultFormat.SampleCount];

					resultFile.ReadSamples(resultData, 0, resultData.Length, deInterleave: false);

					Assert.That(resultData, Is.EqualTo(data), "Data is OK");
				}
				finally
				{
					resultFile.Close();
				}
			}
			finally
			{
				if (File.Exists(name))
				{
					File.Delete(name);
				}
			}
		}

		[Test]
		public void InterleavedRead()
		{
			var name = Path.Combine(Path.GetTempPath(), $"_{Guid.NewGuid():D}.wav");

			try
			{
				var format = new WaveFormat
				{
					Channels = 4,
					SampleFormat = SampleFormat.IEEEFloat,
					SampleRate = 48000,
					SampleSize = sizeof(float)
				};

				var output = new WaveFile(name, format);
				var sampleCount = 1024;
				var frames = sampleCount / format.Channels;

				try
				{
					var data = new float[sampleCount];
					var p = 0;

					for (int f = 0; f < frames; f++)
					{
						for (int c = 0; c < format.Channels; c++)
						{
							data[p++] = c * 1000 + f;
						}
					}
					output.WriteSamples(data, 0, data.Length);
				}
				finally
				{
					output.Close();
				}

				var input = new WaveFile(name);

				try
				{
					var result = new float[sampleCount];

					input.ReadSamples(result, 0, sampleCount, deInterleave: true);

					var p = 0;

					for (int c = 0; c < format.Channels; c++)
					{
						for (int f = 0; f < frames; f++)
						{
							Assert.That(result[p++], Is.EqualTo(c * 1000 + f), "De-interleaved read splits frames and returns channels in sequence.");
						}
					}
				}
				finally
				{
					input.Close();
				}
			}
			finally
			{
				if (File.Exists(name))
				{
					File.Delete(name);
				}
			}
		}
	}
}