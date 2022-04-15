#pragma once

#include "../include/logger.h"

namespace logx {
	Logger::Logger(const char * name) : _name(name) {
		this->_level = LogLevel::DEBUG;
	}

	void Logger::log(LogLevel::Level level, LogEvent::sptr event) {
		if (level < this->_level) return;
		event->loggerName = _name;
		event->level = level;
		for (auto it : _appenders)
			it->log(event);
	}

	void Logger::debug(LogEvent::sptr event) {
		log(LogLevel::DEBUG, event);
	}
	void Logger::info(LogEvent::sptr event) {
		log(LogLevel::INFO, event);
	}
	void Logger::warn(LogEvent::sptr event) {
		log(LogLevel::WARN, event);
	}
	void Logger::error(LogEvent::sptr event) {
		log(LogLevel::ERROR, event);
	}
	void Logger::fatal(LogEvent::sptr event) {
		log(LogLevel::FATAL, event);
	}

	void Logger::addAppender(LogAppender::sptr appender) {
		_appenders.push_back(appender);
	}

	void Logger::delAppender(LogAppender::sptr appender) {
		for (auto it = _appenders.begin(); it != _appenders.end(); ++it)
			if (*it == appender) {
				_appenders.erase(it);
				break;
			}
	}

	void Logger::clear() {
		_name = nullptr;
		_level = LogLevel::DEBUG;
		_pattern = "%d [%p] %t %f:%l %m %n";		// 默认格式解析器
		_appenders.clear();
	}
}