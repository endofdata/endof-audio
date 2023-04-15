#pragma once

#ifndef _AUDIO_VST_UNMANAGED_API
#ifdef AUDIOVSTUNMANAGED_EXPORTS
#define _AUDIO_VST_UNMANAGED_API __declspec(dllexport)
#else
#define _AUDIO_VST_UNMANAGED_API __declspec(dllimport)
#endif
#endif
