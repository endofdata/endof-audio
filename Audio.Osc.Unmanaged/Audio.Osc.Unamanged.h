#pragma once

#ifndef _AUDIO_OSC_UNMANAGED_API
#ifdef AUDIOOSCUNMANAGED_EXPORTS
#define _AUDIO_OSC_UNMANAGED_API __declspec(dllexport)
#else
#define _AUDIO_OSC_UNMANAGED_API __declspec(dllimport)
#endif
#endif
