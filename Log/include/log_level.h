#pragma once

#include <string>

using std::string;

namespace logx {
	/* 日志级别 */
	class LogLevel {
	public:
		enum Level { UNKNOW = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4, FATAL = 5 };

		static const char* getName(LogLevel::Level level) {
			switch (level) {
			case DEBUG: return "debug";
			case INFO: return "info";
			case WARN: return "warn";
			case ERROR: return "error";
			case FATAL: return "fatal";
			default: return "unknow";
			}
		}

		static LogLevel::Level fromString(const string & name) {
			if ("debug" == name) return DEBUG;
			if ("info" == name) return INFO;
			if ("warn" == name) return WARN;
			if ("error" == name) return ERROR;
			if ("fatal" == name) return FATAL;
			return UNKNOW;
		}
	};
}