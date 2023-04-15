[CmdletBinding()]
Param(
	[Parameter(Mandatory=$False)][Int] $DeviceId = 1,
	[Parameter(Mandatory=$False)][Int] $RecordingSeconds = 5,
	[Parameter(Mandatory=$False)][Int] $LoopTimes = 2,
	[Parameter(Mandatory=$False)][String] $MsgRecord = '644AB0',
	[Parameter(Mandatory=$False)][String] $MsgStart = '6449B0',
	[Parameter(Mandatory=$False)][String] $MsgStop = '6447B0',
	[Parameter(Mandatory=$False)][Switch] $StopNow = $False
)

$midiTool = 'D:\Gamer I5\Documents\Projects\Rust\miditool\target\debug\miditool.exe'
$deviceParam = "--device=$DeviceId"

If(!$StopNow) {
	Write-Output "Send first 'Record'"
	& $midiTool  @($deviceParam,$MsgRecord)
	Start-Sleep -Seconds $RecordingSeconds
	Write-Output "Send second 'Record'"
	& $midiTool  @($deviceParam,$MsgRecord)
	Start-Sleep -Seconds $($RecordingSeconds * $LoopTimes)
}
Write-Output "Send 'Stop'"
& $midiTool  @($deviceParam,$MsgStop)
