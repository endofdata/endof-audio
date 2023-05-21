// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <guiddef.h>
#include <math.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <vcclr.h>

// I don't know how to fix the 4996 (ReadOnlySpan<T> is deprecated) 
#pragma warning(disable: 4996)
#endif //PCH_H
