#pragma once

#ifndef _AUDIO_FOUNDATION_UNMANAGED_API
#ifdef AUDIOFOUNDATIONUNMANAGED_EXPORTS
#define _AUDIO_FOUNDATION_UNMANAGED_API __declspec(dllexport)
#else
#define _AUDIO_FOUNDATION_UNMANAGED_API __declspec(dllimport)
#endif
#endif

typedef double Sample;

