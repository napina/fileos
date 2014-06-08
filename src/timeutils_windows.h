/*=============================================================================

Copyright (c) 2013 Ville Ruusutie

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

=============================================================================*/
#pragma once
#ifndef fileos_timeutil_windows_h
#define fileos_timeutil_windows_h

#include "fileos/common.h"
#include "fileos/fileinfo.h"
#include <windows.h>

namespace fileos {

inline void convertTime(SYSTEMTIME const& systemTime, fileos::FileTime& result)
{
    result.year = systemTime.wYear;
    result.month = systemTime.wMonth;
    result.dayOfWeek = systemTime.wDayOfWeek;
    result.day = systemTime.wDay;
    result.hour = systemTime.wHour;
    result.minute = systemTime.wMinute;
    result.second = systemTime.wSecond;
    result.milliseconds = systemTime.wMilliseconds;
}

inline void convertTime(FILETIME const& fileTime, fileos::FileTime& result)
{
    SYSTEMTIME systemTime;
    ::FileTimeToSystemTime(&fileTime, &systemTime);
    convertTime(systemTime, result);
}

} // end of fileos

#endif
