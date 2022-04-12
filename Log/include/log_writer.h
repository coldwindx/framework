#pragma once

#include <stdio.h>  
#include <stdarg.h>  
#include "log_event.h"
#include "log_util.h"
#include "logger.h"


class LogWriter {
public:
	LogWriter(Logger::sptr logger, LogEvent::sptr event) : _logger(logger), _event(event) {}
	~LogWriter() {
		_logger->log(_event->level, _event);
	}

	stringstream& stream() { return _event->ss; }
	stringstream& format(const char * fmt, ...) {
		va_list al;
		va_start(al, fmt);
		char buf[1024];
		int len = vsnprintf(buf, 1024, fmt, al);
		if (len != -1) _event->ss << string(buf, len);
		va_end(al);
	}
private:
	Logger::sptr _logger;
	LogEvent::sptr _event;
};


#define _LOG_LEVEL(logger, level) \
	if(logger->getLevel() <= level) \
		LogWriter(logger, LogEvent::sptr(new LogEvent(__FILE__, __LINE__, level, 0, LogUtil::thread_id(), LogUtil::fiber_id(), time(0)))).stream()
#define _LOG_DEBUG(logger) _LOG_LEVEL(logger, LogLevel::DEBUG)
#define _LOG_INFO(logger) _LOG_LEVEL(logger, LogLevel::INFO)
#define _LOG_WARN(logger) _LOG_LEVEL(logger, LogLevel::WARN)
#define _LOG_ERROR(logger) _LOG_LEVEL(logger, LogLevel::ERROR)
#define _LOG_FATAL(logger) _LOG_LEVEL(logger, LogLevel::FATAL)

#define _LOG_FMT_LEVEL(logger, level, fmt, ...) \
	if(logger->getLevel() <= level) \
		LogWriter(logger, LogEvent::sptr(new LogEvent(__FILE__, __LINE__, level, 0, LogUtil::thread_id(), LogUtil::fiber_id(), time(0)))).format(fmt, __VA_ARGS__)
#define _LOG_FMT_DEBUG(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::DEBUG, fmt, __VA_ARGS__)
#define _LOG_FMT_INFO(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::INFO, fmt, __VA_ARGS__)
#define _LOG_FMT_WARN(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::WARN, fmt, __VA_ARGS__)
#define _LOG_FMT_ERROR(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::ERROR, fmt, __VA_ARGS__)
#define _LOG_FMT_FATAL(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::FATAL, fmt, __VA_ARGS__)
