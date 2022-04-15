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

		void login(const string & name, Logger::sptr logger) {
			_loggers[name] = logger;
		}
		Logger::sptr get(const string & name = "root") {
			auto it = _loggers.find(name);
			if (_loggers.end() != it)
				return it->second;
			Logger::sptr log(new Logger(name.c_str()));
			return _loggers[name] = log;
		}

		void remove(const string & name) {
			_loggers.erase(name);
		}

		static LogManager::sptr instance() {
			static LogManager::sptr p(new LogManager());
			return p;
		}
	private:
		LogManager() {
			Logger::sptr logger = Logger::sptr(new Logger);
			logger->addAppender(LogAppender::sptr(new FileLogAppender("./system.log")));
			_loggers["root"] = logger;
		}

		map<string, Logger::sptr> _loggers;
	};

}