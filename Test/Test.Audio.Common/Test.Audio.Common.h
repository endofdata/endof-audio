#pragma once

#ifndef _TEST_AUDIO_COMMON_API
#ifdef TEST_AUDIO_COMMON_EXPORTS
#define _TEST_AUDIO_COMMON_API _declspec(dllexport)
#else
#define _TEST_AUDIO_COMMON_API _declspec(dllimport)
#endif
#endif