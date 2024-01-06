using Audio.Asio.Interop;
using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;

namespace Test.Audio.Asio
{
	/// <summary>
	/// Creates <see cref="TapeMachine"/> instances using different combinations of ASIO drivers
	/// </summary>
	/// <remarks>
	/// Although this could be used in a <see cref="TestCaseSourceAttribute"/>, doing so causes
	/// COM related issues, that cannot be fixed even by the Appartment attribute.
	/// </remarks>
	/// <seealso href="https://docs.nunit.org/articles/nunit/writing-tests/attributes/apartment.html"/>
	internal class TapeMachineProvider
	{
		public const string SteinbergDriver = "Yamaha Steinberg USB ASIO";
		// Realtek ASIO does not work.
		//public const string RealtekDriver = "Realtek ASIO";
		public const string ASIO4AllDriver = "ASIO4ALL";

		public static IEnumerable<TapeMachine> TapeMachines
		{
			get
			{
				var driverRegistry = DriverRegistry.FromRegistry();

				Assert.That(driverRegistry.Any(), Is.True, "Driver registry contains entries.");

				var firstDriverGuid = GetDriverGuidByName(driverRegistry, SteinbergDriver);
				var secondDriverGuid = GetDriverGuidByName(driverRegistry, ASIO4AllDriver);

				yield return CreateTapeMachine(firstDriverGuid, inputDriver: null);

				yield return CreateTapeMachine(firstDriverGuid, firstDriverGuid);

				yield return CreateTapeMachine(secondDriverGuid, inputDriver: null);

				yield return CreateTapeMachine(secondDriverGuid, secondDriverGuid);
			}
		}

		public static IEnumerable<TapeMachine> MachinesByName(string driverName)
		{
			var driverRegistry = DriverRegistry.FromRegistry();

			Assert.That(driverRegistry.Any(), Is.True, "Driver registry from system registry contains entries.");

			var firstDriverGuid = GetDriverGuidByName(driverRegistry, driverName);

			TestContext.WriteLine($"Driver name: '{driverName}'");

			yield return CreateTapeMachine(firstDriverGuid, inputDriver: null);

			yield return CreateTapeMachine(firstDriverGuid, firstDriverGuid);
		}

		public static TapeMachine CreateTapeMachine(Guid outputDriver, Guid? inputDriver)
		{
			TapeMachine? tapeMachine = null;
			AsioDevice? outputDevice = null;
			AsioDevice? inputDevice = null;

			try
			{
				TestContext.WriteLine($"Creating output device for '{outputDriver}' on thread {System.Threading.Thread.CurrentThread.ManagedThreadId}");

				outputDevice = AsioDevice.CreateFromGuid(outputDriver);

				TestContext.WriteLine($"Output driver name: {outputDevice.DriverName}");

				if (inputDriver.HasValue)
				{
					if (inputDriver.Value != outputDriver)
					{
						TestContext.WriteLine($"Creating input device for '{inputDriver.Value}'");

						inputDevice = AsioDevice.CreateFromGuid(inputDriver.Value);
					}
					else
					{
						TestContext.WriteLine($"Using output device as input device");

						inputDevice = outputDevice;
					}

					TestContext.WriteLine($"Input driver name: {inputDevice.DriverName}");

					var inputChannel = inputDevice.AvailableInputChannels.First();

					Assert.That(inputChannel, Is.Not.Null, "Required input channel available.");

					inputDevice.SelectInputChannel(inputChannel.Key, isSelected: true);
				}

				Assert.That(outputDevice.AvailableOutputChannels.Count, Is.GreaterThanOrEqualTo(2), "Required two output channels are available.");

				var outputChannels = outputDevice.AvailableOutputChannels.Take(2).ToArray();

				outputDevice.SelectOutputChannel(outputChannels[0].Key, isSelected: true);
				outputDevice.SelectOutputChannel(outputChannels[1].Key, isSelected: true);
				outputDevice.ActivateChannels();
				outputDevice.IsPoweredOn = true;

				if (inputDevice != null && inputDevice != outputDevice)
				{
					inputDevice.ActivateChannels();
					inputDevice.IsPoweredOn = true;
				}

				var router = new AsioRouter(outputDevice, inputDevice);

				Assert.That(() => tapeMachine = new TapeMachine(router), Throws.Nothing, "Creating tape machine does not throw.");

				Assert.That(tapeMachine, Is.Not.Null, "TapeMachine is not null.");

				// null-forgiving operator: tapeMachine is Not.Null asserted before
				Assert.That(tapeMachine!.Tracks.Any(), Is.False, "New tape machine has no tracks.");
			}
			catch (Exception)
			{
				if (tapeMachine != null)
				{
					tapeMachine.Dispose();
				}
				if (outputDevice != null)
				{
					outputDevice.Dispose();
				}
				if (inputDevice != null && inputDevice != outputDevice)
				{
					inputDevice.Dispose();
				}
				throw;
			}
			return tapeMachine;
		}

		public static Guid GetDriverGuidByName(DriverRegistry driverRegistry, string pattern)
		{
			Guid? guid = GetDriverGuidFor(driverRegistry, entry => Regex.IsMatch(entry.Name, pattern));

			Assert.That(guid.HasValue, Is.True, $"Driver named like '{pattern}' is avaialble.");

			// null-forgiving operator: guid.HasValue is asserted
			return guid!.Value;
		}

		public static Guid? GetDriverGuidFor(DriverRegistry driverRegistry, Func<RegisteredDriver, bool> predicate) => driverRegistry.FirstOrDefault(predicate)?.ClsId;
	}
}