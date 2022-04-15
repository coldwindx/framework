#pragma once

#include "log_config_define.h"
#include "log_config_convert.h"

using namespace logx;

__CONFIGX_BEGIN__

ConfigVar<set<LogConfig> >::sptr g_log_defines = Config::lookup("logs", set<LogConfig>(), "logs config");


struct LogIniter {
	LogIniter() {
		g_log_defines->addLister(0xF1E231, [](const set<LogConfig>& oldValue
			, const set<LogConfig>& newValue) {
				//_LOG_INFO(_LOG_ROOT()) << "on logger conf changed";
				for (auto & i : newValue) {
					auto it = oldValue.find(i);
					Logger::sptr logger;
					if (it != oldValue.end() && i == *it)
						continue;
					logger = _LOG_NAME(i.name);
					logger->clear();

					logger->setName(i.name.c_str());
					logger->setLevel(i.level);
					if (!i.formatter.empty()) {
						logger->setPattern(i.formatter.c_str());
					}
					for (auto & a : i.appenders) {
						LogAppender::sptr ap;
						if (a.type == "FileLogAppender") {
							ap.reset(new FileLogAppender(a.file));
						}
						if (a.type == "StdoutLogAppender") {
							ap.reset(new StdoutLogAppender);
						}
					
						if (a.level == LogLevel::UNKNOW)
							ap->setLevel(i.level);
						else
							ap->setLevel(a.level);

						string fmt = a.formatter.empty() 
							? logger->getPattern() : a.formatter;
						LogFormatter::sptr formatter(new LogFormatter(fmt));
						if (!formatter->isError())
							ap->setFormatter(formatter);
						else {
							_LOG_ERROR(_LOG_ROOT()) << "logger name=" << i.name
								<< " appender type=" << a.type
								<< " formatter=" << a.formatter
								<< " is invaild" << std::endl;
						}
						logger->addAppender(ap);
					}
				}
				LogManager::sptr p = LogManager::instance();
				for (auto & i : oldValue) {
					auto it = newValue.find(i);
					if (it == newValue.end()) {
						LogManager::instance()->remove(i.name);	// É¾³ý
					}
				}
			});
	}
} __log_init;

__CONFIGX_END__