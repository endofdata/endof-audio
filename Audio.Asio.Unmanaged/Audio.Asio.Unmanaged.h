#pragma once

#ifndef _AUDIO_ASIO_UNMANAGED_API
#ifdef AUDIOASIOUNMANAGED_EXPORTS
#define _AUDIO_ASIO_UNMANAGED_API __declspec(dllexport)
#else
#define _AUDIO_ASIO_UNMANAGED_API __declspec(dllimport)
#endif
#endif

