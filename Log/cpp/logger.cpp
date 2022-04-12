#pragma once

#include "../include/logger.h"

Logger::Logger(const char * name) : _name(name) {
	this->_level = LogLevel::DEBUG;
	this->_formatters.reset(new LogFormatter("%d [%p] %f:%l %m %n"));
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
	if (!appender->getFormatter())
		appender->setFormatter(_formatters);
	_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::sptr appender) {
	for (auto it = _appenders.begin(); it != _appenders.end(); ++it)
		if (*it == appender) {
			_appenders.erase(it);
			break;
		}
}
