[CmdletBinding()]
Param(
	[Parameter(Mandatory=$False)][String] $Configuration = "Release",
	[Parameter(Mandatory=$False)][Switch] $NoBuild = $False
)

Function Pack-Project {
	Param(
		[Parameter(Mandatory=$True)][String] $ProjectName,
		[Parameter(Mandatory=$False)][Switch] $NoBuild
	)
	If(!$NoBuild) {
		& msbuild -property:configuration=$Configuration -property:Platform=x64 ".\${ProjectName}\${ProjectName}.vcxproj"
	}
	& nuget pack ".\${ProjectName}\${ProjectName}.nuspec" -OutputDirectory ".\${ProjectName}\bin\${Configuration}" -BasePath ".\${ProjectName}" -Properties configuration=$Configuration
	Copy-Item ".\${ProjectName}\bin\${Configuration}\*.nupkg" '..\NugetPackages'
}
Push-Location

Try {
	Pack-Project 'Audio.Foundation.Unmanaged' -NoBuild:$NoBuild
	Pack-Project 'Audio.Foundation' -NoBuild:$NoBuild
}
Finally {
	Pop-Location
}
