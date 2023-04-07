[CmdletBinding()]
Param(
	[Parameter(Mandatory=$False)][Int] $DeviceId = 1,
	[Parameter(Mandatory=$False)][String] $MsgRecord = '644AB0',
	[Parameter(Mandatory=$False)][String] $MsgStop = '6447B0'
)

$midiTool = 'D:\Gamer I5\Documents\Projects\Rust\miditool\target\debug\miditool.exe'
$deviceParam = "--device=$DeviceId"

Write-Output "Start recording"
& $midiTool  @($deviceParam,$MsgRecord)
Start-Sleep -Seconds 5
Write-Output "Next part"
& $midiTool  @($deviceParam,$MsgRecord)
Start-Sleep -Seconds 5
Write-Output "Terminating"
& $midiTool  @($deviceParam,$MsgStop)
