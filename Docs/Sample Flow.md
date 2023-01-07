# Audio.Asio Sample Flow

There are three levels of sample handling:
- the unmanaged hardware I/O buffers with device-dependent native samples
- the unmanaged sample buffers containing native samples converted to internal sample type ```float```
- the managed sample buffers as seen by the application

These three levels are represented by the following interfaces:

| Level                                | Interfaces                         |
|--------------------------------------|------------------------------------|
| Native sample buffers (Hardware I/O) | IInputChannel, IOutputChannelPair  |
| Unmanaged sample buffers             | ISampleContainer, ISampleReceiver  |
| Managed sample buffers               | IAudioInput, IAudioOutput          |

## Audio.Foundation.Unmanaged.Abstractions.IInputChannel

Converts single-channel hardware sample buffers to unmanaged two-channel buffers of ```float``` sample format. 
Supports direct-out monitoring of native samples via [IOutputChannelPair](#audiofoundationunmanagedabstractionsioutputchannelpair).

Creates a [Audio.Foundation.Unmanaged.Abstractions.ISampleContainer](#audiofoundationunmanagedabstractionsisamplecontainer) and a
[Audio.Foundation.Unmanaged.Abstractions.ISampleSharer](#audiofoundationunmanagedabstractionsisamplesharer). The ```ISampleSharer.Source```
is set tot the ```ISampleContainer```.

- template in ```Audio.Foundation.Unmanaged\InputOfTSampleChannel.h``` implements ```IInputChannel```
- hardware-sample specific implementation e.g. in ```Audio.Asio\InputInt32Channel.h```

### InputOfTSampleChannel.Swap() invokes...
  - [IOutputChannelPair](#audiofoundationunmanagedabstractionsioutputchannelpair).DirectOut() is called on ```Monitor```, if available to write input samples to hardware output buffers
  - ```InputOfTSampleChannel.ReadSample()``` to convert hardware input buffers to float
  - ```float``` samples are written to both channels of an [ISampleContainer](#audiofoundationunmanagedabstractionsisamplecontainer)

### InputOfTSampleChannel.Send() invokes...
  - [ISampleSharer](#audiofoundationunmanagedabstractionsisamplesharer).RouteToTargets() to forward converted samples to any attached target

## Audio.Foundation.Unmanaged.Abstractions.ISampleContainer

Exposes two channels of sample data as [ISampleBuffer](#audiofoundationunmanagedabstractionsisamplebuffer) instances (```LeftChannel``` and ```RightChannel```). 
Also exposes the buffer size as ```SampleCount```

## Audio.Foundation.Unmanaged.Abstractions.ISampleBuffer

Unmanaged memory buffer for of ```float``` samples that can be accessed as pointer or by indexer. 
Also exposes the buffer size as ```SampleCount```

## Audio.Foundation.Unmanaged.Abstractions.ISampleSharer

Manages the connection between an [ISampleContainer](#audiofoundationunmanagedabstractionsisamplecontainer) *Source* and a collection of 
[Audio.Foundation.Unmanaged.Abstractions.ISampleReceiver](#audiofoundationunmanagedabstractionsisamplereceiver) *Targets*.

### ISampleSharer.RouteToSends() invokes...
- ```ISampleReceiver.Receive()``` for each ```ISampleReceiver```, passing the same ```ISampleContainer``` as input source

## Audio.Foundation.Unmanaged.Abstractions.ISampleReceiver

Exposes the ```Receive(ISampleContainer)``` method to receive sample data


## Audio.Foundation.Abstractions.IAudioInput

Moves the unmanaged sample buffers to managed memory. Level metering and optional monitoring for input channel

- implementation in ```Audio.Asio.AudioInput```
- reads samples from ```IInputChannel```
- attaches level metering to input channel:
  - creates ```Audio.Foundation.Unmanaged.IMeterChannel``` for continuous update of ```IAudioInput.DbFS``` property
  - links the ```IInputChannel.SampleSharer``` to the ```ISampleReceiver``` of the ```IMeterChannel```
- ```IAudioInput.ReadCurrentFrame()``` copies the unmanaged sample buffer of its ```IInputChannel.SampleContainer``` to a managed ```float[]``` **but only the left channel**
- setting the ```IAudioInput.Monitor``` requires that the monitor ```IAudioOutput``` is an ```Audio.Asio.Interop.AudioOutput``` and assigns its unmanaged ```IOutputChannelPair``` to ```IAudioInput.Monitor```

## Audio.Asio.Interop.AudioTrack

Manages a collection of ```Audio.Foundation.Abstractions.IAudioTake``` instances on a timeline. New takes can be recorded. Supports monitoring also. Very basic implementation w/o support for overlapping items yet.

The behavior is controlled by the track status:

|Status    |Description                                                    |
|----------|---------------------------------------------------------------|
|cold      |Track plays back previously recorded takes                     |
|hot       |Track receives input from its ```RecordIn```                   |
|recording |Track writes received input to its recording take              |

The *hot* state is set by arming a track for recording. It is designed to adjust the recording level. Only in the *recording* state, input is actually recorded.

### Relevant properties and members
- ```Audio.Foundation.Abstractions.IAudioInput RecordIn```: input for recording
- ```Audio.Foundation.Abstractions.IAudioOutput MonitorOut```: output for monitoring
- ```Audio.Foundation.Interop.AudioRecording m_recordingTake```: current recording take
- ```Audio.Foundation.Abstractions.IAudioTake m_playbackTake```: current playback take

### AudioTake.NextFrame() invokes...
- when track is *cold*: ```IAudioTake.ReadNextFrame()``` on ```m_playbackTake```, if any
- when track is *hot*: ```IAudioInput.ReadNextFrame()``` on ```RecordIn```
- when track is *recording*: ```AudioRecording.WriteNextFrame()``` on ```m_recordingTake```

Wether a playback-take is available depends on the song position and previously recorded takes.

After ```AudioTrack.NextFrame()``` completes, the internal buffer ```float[] m_pCurrentFrame``` is filled.

When ```AudioTrack.Send()``` is invoked, and the track is not muted, the current frame is written to ```MonitorOut``` if any.


## Audio.Asio.Interop.TapeMachine

Manages a collection of ```AudioTrack``` instances as well as the global transport and recording status. Receives buffer-switch notification from an ```Audio.Asio.Interop.AsioRouter```.

- invokes ```AudioTrack.NextFrame()``` and ```AudioTrack.Send()```
  - for *hot* tracks in ```TapeMachine.OnInputBufferSwitch```
  - for *cold* tracks in ```TapeMachine.OnOutputBufferSwitch```


## Audio.Asio.Interop.AsioRouter

Combines ```Audio.Asio.Interop.AsioDevice``` instances for input and output. Propagates buffer-switch notifications as received by the devices. Creates the managed ```Audio.Foundation.Abstractions.IAudioInput``` and ```IAudioOutput``` for ```Audio.Foundation.Unmanaged.IInputChannel```s and ```IOutputChannelPair```s of the devices. Controls the global device status (*powerded on/off*).

- ```AttachBufferSwitchHandler``` allows to set ```Audio.Asio.Interop.BufferSwitchManagedCallback```s for input and output buffer-switch notifications
- ```OnInputBufferSwitch``` 
  - is attached to the input device's unmanaged buffer-switch notifications
  - propagates the notification to the managed ```BufferSwitchManagedCallback``` for input

- ```OnOutputBufferSwitch``` 
  - is attached to the output device's unmanaged buffer-switch notifications
  - propagates the notification to the managed ```BufferSwitchManagedCallback``` for output
  - calls ```AudioOutput.Send()``` for each two-channel audio output

## Audio.Asio.Interop.AudioOutput

Mixes the internal sample buffers and forwards them to an unmanaged ```IOutputChannelPair```. Continuous level metering not implemented yet.

- creates a ```Audio.Foundation.Unmanaged.SampleJoiner``` to mix down all frames written during a single buffer switch
- sets the ```ISampleJoiner.Target``` to the ```IOutputChannelPair```'s ```ISampleReceiver``` implementation

### AudioOutput.WriteNextFrame() invokes...
- ```ISampleJoine.MixInput()``` to add the frame to the combined output mix

### AudioOutput.Send() invokes...
- ```ISampleJoiner.Send()``` to write the mix to the output channel
- ```ISampleJoiner.Flush()``` to reset the mix

## Audio.Foundation.Unmanaged.Abstractions.ISampleJoiner

Mix down multiple two-channel sample buffers and send them to a native ```IOutputChannelPair```

### SampleJoiner.Send() invokes...
- ```IChannelLink.Output.Receive()``` on its ```OutputLink```, passing the output link as parameter also

### SampleJoiner.Receive() invokes...
- ```SampleJoiner.MixInput()```

## Audio.Foundation.Unmanaged.Abstractions.IOutputChannelPair

Exposes ```Swap(bool writeSecondHalf)``` and ```DirectOut(...)``` methods.
Also exposes a ```int SampleType``` property.

- template in ```Audio.Foundation.Unmanaged.OutputOfTSampleChannelPair.h``` implements [ISampleReceiver](#audiofoundationunmanagedabstractionsisamplereceiver) and ```IOutputChannelPair```
- hardware-sample specific implementation e.g. in ```Audio.Asio\OutputInt32ChannelPair.h```

### OutputOfTSampleChannelPair.Swap() invokes...
- nothing, but stores the ```writeSecondHalf``` flag for use in ```Receive()```

### OutputOfTSampleChannelPair.Flush() invokes...
- nothing

### OutputOfTSampleChannelPair.Receive() invokes...
- ```OutputOfTSampleChannelPair.WriteSample()``` to write the converted ```float``` samples to unmanaged hardware sample buffers

# Questions and remarks

In some places where ```IChannelLink``` is used, the callee takes either the ```Input``` or the ```Output``` but not both. For a clean separation, use only ```ISampleContainer``` as input or ```ISampleReceiver``` as output.

We have an active pushing of samples with ```InputChannel.Send``` but also a passive pulling of samples with ```IAudioInput.ReadCurrentFrame()```. The active path is used for the monitoring only. Can we use a *pulling* monitor implementation instead?

Rename ```IAudioTake.ReadNextFrame()``` to ```ReadCurrentFrame()``` as in ```IAudioInput```

Why is the ```ISampleSharer``` handled by native ```IInputChannel``` but the ```ISampleJoiner``` is handled by managed ```IAudioOutput```?

Why does the ```SampleJoiner``` provide two (or three) methods to add samples to the mixdown (Receive and 2x MixInput)?
