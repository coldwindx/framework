#pragma once

#include <memory>
#include <list>
#include "log_level.h"
#include "log_event.h"
#include "log_appender.h"

using std::list;

namespace logx {
	class Logger : public std::enable_shared_from_this<Logger> {
	public:
		typedef std::shared_ptr<Logger> sptr;

		Logger(const char * name = "root");

		void log(LogLevel::Level level, LogEvent::sptr event);
		void debug(LogEvent::sptr event);
		void info(LogEvent::sptr event);
		void warn(LogEvent::sptr event);
		void error(LogEvent::sptr event);
		void fatal(LogEvent::sptr event);

		void addAppender(LogAppender::sptr appender);
		void delAppender(LogAppender::sptr appender);

		LogLevel::Level getLevel() const { return _level; }
		void setLevel(LogLevel::Level level) { _level = level; }
		const char * getName() const { return _name; }
	private:
		const char * _name = nullptr;
		LogLevel::Level _level = LogLevel::DEBUG;
		list<LogAppender::sptr> _appenders;
		LogFormatter::sptr _formatters;
	};
}