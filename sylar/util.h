#pragma once

#include <stdint.h>
#include <thread>
#include <sstream>

using std::stringstream;

static uint32_t GetThreadId() {
	stringstream ss;
	ss << std::this_thread::get_id();
	return std::stoull(ss.str());
}

static uint32_t GetFiberId() {
	return 0;
}