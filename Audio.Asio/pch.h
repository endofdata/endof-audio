#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <asio.h>
//#include <iasiodrv.h>
#include <guiddef.h>
#include <math.h>
#include <algorithm>
#include <functional>
#include <vector>
#include <vcclr.h>

// I don't know how to fix the 4996 (ReadOnlySpan<T> is deprecated) 
#pragma warning(disable: 4996)