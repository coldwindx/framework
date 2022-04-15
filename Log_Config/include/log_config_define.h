#pragma once

#include <log.h>
#include <config.h>
#include <yaml-cpp\yaml.h>

#pragma comment(lib, "Log.lib")
#pragma comment(lib, "Config.lib")
#pragma comment(lib, "yaml-cppd.lib")

#define __LOGX__ namespace logx{
#define __END__ }

__LOGX__
struct LogAppenderConfig {
	string type;
	LogLevel::Level level = LogLevel::UNKNOW;
	string formatter;
	string file;

	bool operator==(const LogAppenderConfig & oth) const {
		return type == oth.type && level == oth.level
			&& formatter == oth.formatter && file == oth.file;
	}
};

struct LogConfig {
	string name;
	LogLevel::Level level = LogLevel::UNKNOW;
	string formatter;
	vector<LogAppenderConfig> appenders;

	bool operator==(const LogConfig & oth) const {
		return name == oth.name && level == oth.level
			&& formatter == oth.formatter && appenders == oth.appenders;
	}

	bool operator<(const LogConfig& oth) const {
		return name < oth.name;
	}
};
__END__