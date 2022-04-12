#pragma once

#include <stdint.h>
#include <thread>
#include <sstream>
#include <time.h>

using std::stringstream;

class LogUtil {
public:
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

	static uint32_t thread_id() {
		stringstream ss;
		ss << std::this_thread::get_id();
		return std::stoull(ss.str());
	}

	static uint32_t fiber_id() {
		return 0;
	}
};
