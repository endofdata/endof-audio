#include "pch.h"
#include "AudioTargetBase.h"

using namespace Audio::Foundation::Interop;

AudioTargetBase::AudioTargetBase()
{
}

AudioTargetBase::~AudioTargetBase()
{
}

int AudioTargetBase::Write(IAudioBuffer^ buffer)
{
	return OnWrite(buffer);
}

int AudioTargetBase::OnWrite(IAudioBuffer^ buffer)
{
	return 0;
}
