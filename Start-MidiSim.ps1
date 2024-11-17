[CmdletBinding()]
Param(
	[Parameter(Mandatory=$False)][Int] $DeviceId = 1,
	[Parameter(Mandatory=$False)][Int] $RecordingSeconds = 5,
	[Parameter(Mandatory=$False)][Int] $LoopTimes = 2,
	[Parameter(Mandatory=$False)][String] $MsgLocate = '6446B0',
	[Parameter(Mandatory=$False)][String] $MsgPause = '6447B0',
	[Parameter(Mandatory=$False)][String] $MsgCancel = '6448B0',
	[Parameter(Mandatory=$False)][String] $MsgRecord = '644AB0',
	[Parameter(Mandatory=$False)][String] $MsgStart = '6450B0',
	[Parameter(Mandatory=$False)][String] $MsgStop = '6451B0',
	[Parameter(Mandatory=$False)][Switch] $StopNow = $False
)

# For message values refer to
# .\Audio.Foundation.Unmanaged\MidiTransportControl.cpp
$midiTool = "$PSScriptRoot\..\Rust\miditool\target\debug\miditool.exe"
$deviceParam = "--device=$DeviceId"

If(!$StopNow) {
	Write-Output "Send first 'Record'. Should be recording..."
	& $midiTool  @($deviceParam,$MsgRecord)
	Start-Sleep -Seconds $RecordingSeconds

	Write-Output "Send second 'Record'. Should be looping $LoopTimes times..."
	& $midiTool  @($deviceParam,$MsgRecord)
	Start-Sleep -Seconds $($RecordingSeconds * $LoopTimes)

	For($i = 0; $1 -lt $3; $i++) {
		Start-Sleep -Seconds 0.5
		Write-Output "Send 'Locate' ($i). Should restart loop..."
		& $midiTool  @($deviceParam,$MsgLocate)
	}

	Start-Sleep -Seconds 0.5
	Write-Output "Send first 'Pause'. Should suspend..."
	& $midiTool  @($deviceParam,$MsgPause)

	Start-Sleep -Seconds $RecordingSeconds
	Write-Output "Send second 'Pause'. Should resume..."
	& $midiTool  @($deviceParam,$MsgPause)	

	Write-Output "Playing along. Should play $RecordingSeconds s..."
	Start-Sleep -Seconds $RecordingSeconds
}
Write-Output "Send 'Stop'"
& $midiTool  @($deviceParam,$MsgStop)
