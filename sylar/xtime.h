#pragma once

#include <time.h>

static void xlocaltime(time_t & time, struct tm & tm) {
#ifdef _WIN32
	localtime_s(&tm, &time);
#endif // _WIN32
#ifdef __WINDOWS_
	localtime_s(&tm, &time);
#endif // __WINDOWS_
#ifdef linux
	localtime_r(&time, &tm);
#endif // linux
}