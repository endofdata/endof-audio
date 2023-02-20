// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <stdexcept>
#include <string>
#include <ostream>
#include <istream>
#include <fstream>

// I don't know how to fix the 4996 (ReadOnlySpan<T> is deprecated) and I don't care for 4251 (no dll interface for com_ptr_t<>)
#pragma warning(disable: 4996 4251)
#endif //PCH_H
