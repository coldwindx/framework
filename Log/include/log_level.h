#pragma once

namespace logx {
	/* 日志级别 */
	class LogLevel {
	public:
		enum Level { UNKNOW = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4, FATAL = 5 };

		static const char* getName(LogLevel::Level level) {
			switch (level) {
			case UNKNOW: return "unknow";
			case DEBUG: return "debug";
			case INFO: return "info";
			case WARN: return "warn";
			case ERROR: return "error";
			case FATAL: return "fatal";
			}
		}
	};
}