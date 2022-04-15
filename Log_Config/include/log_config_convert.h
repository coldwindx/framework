#pragma once

#include "log_config_define.h"

template<>
class Convert<string, LogConfig> {
public:
	LogConfig operator()(const string& v) {
		YAML::Node n = YAML::Load(v);
		LogConfig ld;
		if (!n["name"].IsDefined()) {
			//std::cout << "log config error: name is null, " << n
			//	<< std::endl;
			throw std::logic_error("log config name is null");
		}
		ld.name = n["name"].as<string>();
		ld.level = LogLevel::fromString(n["level"].IsDefined() ? n["level"].as<string>() : "");
		if (n["formatter"].IsDefined()) {
			ld.formatter = n["formatter"].as<string>();
		}

		if (n["appenders"].IsDefined()) {
			for (size_t x = 0; x < n["appenders"].size(); ++x) {
				auto a = n["appenders"][x];
				if (!a["type"].IsDefined()) {
					throw std::logic_error("log config error: appender type is null in logger= " + ld.name);
					continue;
				}
				LogAppenderConfig lad;
				lad.type = a["type"].as<string>();
				if (lad.type == "FileLogAppender") {
					if (!a["file"].IsDefined()) {
						throw std::logic_error("log config error: fileappender file is null in logger=" + ld.name);
						continue;
					}
					lad.file = a["file"].as<string>();
					if (a["formatter"].IsDefined()) {
						lad.formatter = a["formatter"].as<string>();
					}
				}
				else if (lad.type == "StdoutLogAppender") {
					if (a["formatter"].IsDefined()) {
						lad.formatter = a["formatter"].as<string>();
					}
				}
				else {
					throw std::logic_error("log config error: appender type is invalid in logger=" + ld.name);
					continue;
				}

				if (a["level"].IsDefined())
					lad.level = LogLevel::fromString(a["level"].as<string>());
				else
					lad.level = ld.level;

				ld.appenders.push_back(lad);
			}
		}
		return ld;
	}
};

template<>
class Convert<LogConfig, string> {
public:
	string operator()(const LogConfig& i) {
		YAML::Node n(YAML::NodeType::Map);
		n["name"] = i.name;
		if (i.level != LogLevel::UNKNOW) {
			n["level"] = LogLevel::getName(i.level);
		}
		if (!i.formatter.empty()) {
			n["formatter"] = i.formatter;
		}

		for (auto& a : i.appenders) {
			YAML::Node na(YAML::NodeType::Map);
			na["type"] = "FileLogAppender";
			if(!a.file.empty()) na["file"] = a.file;
			if (a.level != LogLevel::UNKNOW) {
				na["level"] = LogLevel::getName(a.level);
			}

			if (!a.formatter.empty()) {
				na["formatter"] = a.formatter;
			}

			n["appenders"].push_back(na);
		}
		stringstream ss;
		ss << n;
		return ss.str();
	}
};
