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
				var driverRegistry = new DriverRegistry();

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
			var driverRegistry = new DriverRegistry();

			Assert.That(driverRegistry.Any(), Is.True, "Driver registry contains entries.");

			var firstDriverGuid = GetDriverGuidByName(driverRegistry, driverName);

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
				outputDevice = AsioDevice.CreateFromGuid(outputDriver);
				outputDevice.IsPoweredOn = true;

				if (inputDriver.HasValue)
				{
					if (inputDriver.Value != outputDriver)
					{
						inputDevice = AsioDevice.CreateFromGuid(inputDriver.Value);
					}
					else
					{
						inputDevice = outputDevice;
					}
				}

				var router = new AsioRouter(outputDevice, inputDevice);

				Assert.That(() => tapeMachine = new TapeMachine(router), Throws.Nothing, "Creating tape machine does not throw.");

				Assert.That(tapeMachine, Is.Not.Null, "TapeMachine is not null.");

				// null-forgiving operator: tapeMachine is Not.Null asserted before
				Assert.IsFalse(tapeMachine!.Tracks.Any(), "New tape machine has no tracks.");
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