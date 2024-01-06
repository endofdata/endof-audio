using Lupus.Model;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using System;
using System.IO;
using System.Reflection;
using System.Text.Json;
using System.Windows;

namespace Lupus
{
	public partial class App : Application
	{
		const string PrimarySettingsName = "appsettings.json";
		static readonly string EnvironmentSpecificSettingsName = $"appsettings.{GetEnvironmentName()}.json";

		internal static string GetAppBase() => Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) ?? Environment.CurrentDirectory;
		internal static string GetEnvironmentName() => Environment.GetEnvironmentVariable("DOTNET_ENVIRONMENT") ?? "Production";

		private IServiceProvider? _services;

		public IServiceProvider Services
		{
			get => _services ?? throw new InvalidOperationException($"'{typeof(App)}.{nameof(Services)}' are not available before {nameof(OnStartup)} has run.");
			private set => _services = value;
		}

		internal void UpdateConfig(DeviceSelectionModel model)
		{
			try
			{
				var appBase = GetAppBase();
				var envSpecific = Path.Combine(appBase, EnvironmentSpecificSettingsName);
				var settingsName = File.Exists(envSpecific) ? envSpecific : Path.Combine(appBase, PrimarySettingsName);

				using var stream = new FileStream(settingsName, FileMode.OpenOrCreate, FileAccess.ReadWrite);

				var appSettings = JsonSerializer.Deserialize<AppSettings>(stream) ?? new AppSettings();

				appSettings.UpdateDeviceSettings(model);

				stream.Position = 0;
				stream.SetLength(0);

				JsonSerializer.Serialize(stream, appSettings, new JsonSerializerOptions
				{
					WriteIndented = true,
					PropertyNamingPolicy = JsonNamingPolicy.CamelCase
				});

				stream.Flush();
				Services.GetRequiredService<IConfigurationRoot>().Reload();
			}
			catch (Exception ex) when
			(
				ex is IOException ||
				ex is UnauthorizedAccessException || 
				ex is NotSupportedException ||
				ex is JsonException
			)
			{
				throw new InvalidOperationException("Failed to update configuration file.", ex);
			}
		}

		protected override void OnStartup(StartupEventArgs e)
		{
			// this seems to be the only way to make .NET find a native dependency of a managed assembly
			var basePath = GetAppBase();
			var path = Environment.GetEnvironmentVariable("PATH");
			path += $";{basePath}\\runtimes\\win-x64\\native";

			Environment.SetEnvironmentVariable("PATH", path);

			Services = CreateServices();

			base.OnStartup(e);
		}

		protected override void OnExit(ExitEventArgs e)
		{
			base.OnExit(e);
		}

		private static ServiceProvider CreateServices()
		{
			var config = new ConfigurationBuilder()
				.SetBasePath(GetAppBase())
				.AddJsonFile(PrimarySettingsName, optional: false, reloadOnChange: false)
				.AddJsonFile(EnvironmentSpecificSettingsName, optional: true, reloadOnChange: false)
				.Build();
			
			var services = new ServiceCollection();

			services
				.AddSingleton(config)
				//.AddOptions<AppSettings>().BindConfiguration("");
				.Configure<AppSettings>(config);

			return services.BuildServiceProvider();
		}
	}
}
