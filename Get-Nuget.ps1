[CmdletBinding()]
Param(
	[Parameter(Mandatory=$False)][String] $Configuration = "Release",
	[Parameter(Mandatory=$False)][Switch] $NoBuild = $False
)

$ErrorActionPreference = 'Stop'

Function Get-Package {
	Param(
		[Parameter(Mandatory=$True)][String] $ProjectName,
		[Parameter(Mandatory=$False)][Switch] $NoBuild
	)
	Write-Verbose "Packing project '$ProjectName'"
	If(!$NoBuild) {
		& msbuild -property:configuration=$Configuration -property:Platform=x64 -property:SolutionDir="$PSScriptRoot\"  ".\${ProjectName}\${ProjectName}.vcxproj"
	}
	& nuget pack ".\${ProjectName}\${ProjectName}.nuspec" -OutputDirectory ".\${ProjectName}\bin\${Configuration}" -BasePath ".\${ProjectName}" -Properties "Configuration=$Configuration;Platform=x64"
	Copy-Item ".\${ProjectName}\bin\${Configuration}\*.nupkg" '..\NugetPackages'
}
Push-Location

Try {
	Get-Package 'Audio.Foundation.Unmanaged' -NoBuild:$NoBuild
	Get-Package 'Audio.Foundation' -NoBuild:$NoBuild
	Get-Package 'Audio.Vst.Unmanaged' -NoBuild:$NoBuild
	Get-Package 'Audio.Asio.Unmanaged' -NoBuild:$NoBuild
	Get-Package 'Audio.Asio' -NoBuild:$NoBuild
}
Finally {
	Pop-Location
}
