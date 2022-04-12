#pragma once

#include <map>
#include <functional>
#include "logger.h"

using std::map;
using std::function;

namespace logx {
	class LogManager {
	public:
		typedef shared_ptr<LogManager> sptr;

		LogManager() {
			Logger::sptr logger = Logger::sptr(new Logger);
			logger->addAppender(LogAppender::sptr(new StdoutLogAppender));
			_loggers["main"] = logger;
		}

		void login(const string & name, Logger::sptr logger) {
			_loggers[name] = logger;
		}
		Logger::sptr get(const string & name = "main") {
			return _loggers[name];
		}
	private:
		map<string, Logger::sptr> _loggers;
	};

}