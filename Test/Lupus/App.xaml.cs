using System;
using System.IO;
using System.Reflection;
using System.Windows;

namespace Lupus
{
	public partial class App : Application
	{
		protected override void OnStartup(StartupEventArgs e)
		{
			// this seems to be the only way to make .NET find a native dependency of a managed assembly
			var basePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
			var path = Environment.GetEnvironmentVariable("PATH");
			path += $";{basePath}\\runtimes\\win-x64\\native";

			Environment.SetEnvironmentVariable("PATH", path);

			base.OnStartup(e);
		}

		protected override void OnExit(ExitEventArgs e)
		{
			base.OnExit(e);
		}
	}
}
