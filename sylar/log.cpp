#include <stdio.h>  
#include <stdarg.h>  
#include <yaml-cpp\yaml.h>
#include "log.h"
#include "config.h"

#pragma comment(lib, "yaml-cppd.lib")

namespace sylar {

	LogEvent::LogEvent(shared_ptr<Logger> logger, LogLevel::Level level, const char * file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time, const string & threadName) {
		this->_file = file;
		this->_line = line;
		this->_elapse = elapse;
		this->_threadId = threadId;
		this->_fiberId = fiberId;
		this->_time = time;
		this->_threadName = threadName;
		this->_logger = logger;
		this->_level = level;
	}

	void LogEvent::format(const char * fmt, ...)
	{
		va_list al;
		va_start(al, fmt);
		char buf[1024];
		int len = vsnprintf(buf, 1024, fmt, al);
		if (len != -1) _ss << string(buf, len);
		va_end(al);
	}

	/* ----------------------------begin [Logger] begin-------------------------------- */

	Logger::Logger(const string & name) : _name(name) {
		this->_level = LogLevel::DEBUG;
		this->_formatter.reset(new LogFormatter("%d [%p] %f:%l %m %n"));
		//if (name == "root") {
		//	_appenders.push_back(LogAppender::ptr(new StdoutLogAppender));
		//}
	}

	void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
		if (level < this->_level) return;
		auto self = shared_from_this();
		if (!_appenders.empty())
			for (auto it : _appenders)
				it->log(self, level, event);
		else if (_root) {
			_root->log(level, event);
		}
	}

	void Logger::debug(LogEvent::ptr event) {
		log(LogLevel::DEBUG, event);
	}
	void Logger::info(LogEvent::ptr event) {
		log(LogLevel::INFO, event);
	}
	void Logger::warn(LogEvent::ptr event) {
		log(LogLevel::WARN, event);
	}
	void Logger::error(LogEvent::ptr event) {
		log(LogLevel::ERROR, event);
	}
	void Logger::fatal(LogEvent::ptr event) {
		log(LogLevel::FATAL, event);
	}

	void Logger::addAppender(LogAppender::ptr appender) {
		if (!appender->getFormatter())
			appender->_formatter = _formatter;
		_appenders.push_back(appender);
	}

	void Logger::delAppender(LogAppender::ptr appender) {
		for (auto it = _appenders.begin(); it != _appenders.end(); ++it)
			if (*it == appender) {
				_appenders.erase(it);
				break;
			}
	}

	void Logger::clearAppenders()
	{
		_appenders.clear();
	}

	void Logger::setFormatter(LogFormatter::ptr val)
	{
		_formatter = val;
		for (auto & p : _appenders) {
			if (!p->_hasFormatter) {
				p->_formatter = _formatter;
			}
		}
	}

	void Logger::setFormatter(const string & val)
	{
		LogFormatter::ptr format(new LogFormatter(val));
		if (format->isError()) {
			std::cout << "Logger setFormatter name=" << _name
				<< " value=" << val << "invaild formatter" << std::endl;
			return;
		}
		this->setFormatter(format);
	}

	string Logger::toYamlString()
	{
		YAML::Node node(YAML::NodeType::Map);
		node["name"] = _name;
		node["level"] = LogLevel::toString(_level);
		if (_formatter) {
			node["formatter"] = _formatter->getPattern();
		}
		for (auto & i : _appenders) {
			node["appenders"].push_back(YAML::Load(i->toYamlString()));
		}
		stringstream ss;
		ss << node;
		return ss.str();
	}

	/* ----------------------------end [Logger] end-------------------------------- */

	/* ----------------------------begin [StdoutLogAppender] begin-------------------------------- */

	void StdoutLogAppender::log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
		if (level < this->_level) return;
		std::cout << _formatter->format(logger, level, event) << std::endl;
	}

	string StdoutLogAppender::toYamlString()
	{
		YAML::Node node(YAML::NodeType::Map);
		node["type"] = "StdoutLogAppender";
		node["level"] = LogLevel::toString(_level);
		if (_hasFormatter && _formatter) {
			node["formatter"] = _formatter->getPattern();
		}
		stringstream ss;
		ss << node;
		return ss.str();
	}

	/* ----------------------------end [StdoutLogAppender] end-------------------------------- */

	/* ----------------------------begin [FileLogAppender] begin-------------------------------- */

	FileLogAppender::FileLogAppender(const string & filename) : _filename(filename) {
	}

	void FileLogAppender::log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
		if (level < this->_level) return;
		this->reopen();
		_filestream << _formatter->format(logger, level, event) << std::endl;
	}

	bool FileLogAppender::reopen() {
		if (_filestream) _filestream.close();
		_filestream.open(_filename);
		return !_filestream;
	}

	string FileLogAppender::toYamlString()
	{
		YAML::Node node(YAML::NodeType::Map);
		node["type"] = "FileLogAppender";
		node["file"] = _filename;
		node["level"] = LogLevel::toString(_level);
		if (_hasFormatter && _formatter) {
			node["formatter"] = _formatter->getPattern();
		}
		stringstream ss;
		ss << node;
		return ss.str();
	}

	/* ----------------------------end [FileLogAppender] end-------------------------------- */

	/* ----------------------------begin [LogFormatter] begin-------------------------------- */

	LogFormatter::LogFormatter(const string & pattern) : _pattern(pattern) {
		this->init();
	}


	//%xxx %xxx{xxx} %%
	// %m -- 消息体
	// %p -- 等级
	// %r -- 启动后的时间
	// %c -- 日志名称
	// %t -- 线程ID
	// %n -- 回车换行
	// %d -- 时间
	// %f -- 文件名
	// %l -- 行号
	void LogFormatter::init() {
		vector<tuple<string, string, int>> vec;		// str, format, type
		string nstr;

		int i, j, n = _pattern.size();
		for (i = 0; i < n; ++i) {
			if ('%' != _pattern[i]) {
				nstr.append(1, _pattern[i]);
				continue;
			}
			if (i + 1 < n && '%' == _pattern[i + 1]) {
				nstr.append(1, '%');
				++i;
				continue;
			}

			bool status = false;		// status - false:转译串 true:格式串
			int fmtBegin = 0;

			string pat, fmt;
			for (j = i + 1; j < n; ++j) {
				if (!status && (!isalpha(_pattern[j])) && _pattern[j] != '{' && _pattern[j] != '}') {
					pat = _pattern.substr(i + 1, j - i - 1);
					break;
				}
				if (!status) {
					if ('{' == _pattern[j]) {
						pat = _pattern.substr(i + 1, j - i - 1);
						status = true;
						fmtBegin = j;
						continue;
					}
				}
				if (status) {
					if ('}' == _pattern[j]) {
						fmt = _pattern.substr(fmtBegin, j - fmtBegin - 1);
						status = false;
						++j;
						break;
					}
				}
			}

			if (j == n && pat.empty())
				pat = _pattern.substr(i + 1);

			if (!status) {
				if (!nstr.empty()) {
					vec.push_back(std::make_tuple(nstr, string(), 0));
					nstr.clear();
				}
				vec.push_back(std::make_tuple(pat, fmt, 1));
				i = j - 1;
			}
			if (status) {
				std::cout << "pattern parse error: " << _pattern << " - " << _pattern.substr(i) << std::endl;
				_error = true;
				vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
			}
		}
		if (!nstr.empty())
			vec.push_back(std::make_tuple(nstr, string(), 0));

		static map<string, function<FormatItem::ptr(const string &)>> s_format_items = {
	#define XX(str, C) { #str, [](const string & fmt) { return FormatItem::ptr(new C(fmt)); }}
			XX(m, MessageFormatItem),
			XX(p, LevelFormatItem),
			XX(r, ElapseFormatItem),
			XX(c, LoggerFormatItem),
			XX(t, ThreadIdFormatItem),
			XX(n, EnterFormatItem),
			XX(d, DateTimeFormatItem),
			XX(f, FilenameFormatItem),
			XX(l, LineFormatItem),
			XX(T, TabFormatItem),
			XX(F, FiberIdFormatItem)
	#undef XX
		};

		for (auto & i : vec) {
			if (0 == std::get<2>(i)) {
				_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));

				continue;
			}
			auto it = s_format_items.find(std::get<0>(i));
			if (s_format_items.end() == it) {
				_items.push_back(FormatItem::ptr(new StringFormatItem("<<error format %" + std::get<0>(i) + ">>")));
				_error = true;
			}
			else
				_items.push_back(it->second(std::get<1>(i)));

			//std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
		}
	}

	string LogFormatter::format(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
	{
		stringstream os;
		for (auto i : _items)
			i->format(os, logger, level, event);
		return os.str();
	}
	/* ----------------------------end [LogFormatter] end-------------------------------- */

	struct LogAppenderDefine {
		int type = 0;	// 1 - file, 2 - stdout
		LogLevel::Level level = LogLevel::UNKNOW;
		string formatter;
		string file;

		bool operator==(const LogAppenderDefine & oth) const {
			return type == oth.type && level == oth.level
				&& formatter == oth.formatter && file == oth.file;
		}
	};

	struct LogDefine {
		string name;
		LogLevel::Level level = LogLevel::UNKNOW;
		string formatter;
		vector<LogAppenderDefine> appenders;

		bool operator==(const LogDefine & oth) const {
			return name == oth.name && level == oth.level
				&& formatter == oth.formatter && appenders == oth.appenders;
		}

		bool operator<(const LogDefine& oth) const {
			return name < oth.name;
		}
	};


		template<>
		class LeixiCast<string, LogDefine> {
		public:
			LogDefine operator()(const string& v) {
				YAML::Node n = YAML::Load(v);
				LogDefine ld;
				if (!n["name"].IsDefined()) {
					std::cout << "log config error: name is null, " << n
						<< std::endl;
					throw std::logic_error("log config name is null");
				}
				ld.name = n["name"].as<string>();
				ld.level = LogLevel::fromString(n["level"].IsDefined() ? n["level"].as<string>() : "");
				if (n["formatter"].IsDefined()) {
					ld.formatter = n["formatter"].as<string>();
				}

				if (n["appenders"].IsDefined()) {
					//std::cout << "==" << ld.name << " = " << n["appenders"].size() << std::endl;
					for (size_t x = 0; x < n["appenders"].size(); ++x) {
						auto a = n["appenders"][x];
						if (!a["type"].IsDefined()) {
							std::cout << "log config error: appender type is null, " << a
								<< std::endl;
							continue;
						}
						string type = a["type"].as<string>();
						LogAppenderDefine lad;
						if (type == "FileLogAppender") {
							lad.type = 1;
							if (!a["file"].IsDefined()) {
								std::cout << "log config error: fileappender file is null, " << a
									<< std::endl;
								continue;
							}
							lad.file = a["file"].as<string>();
							if (a["formatter"].IsDefined()) {
								lad.formatter = a["formatter"].as<string>();
							}
						}
						else if (type == "StdoutLogAppender") {
							lad.type = 2;
							if (a["formatter"].IsDefined()) {
								lad.formatter = a["formatter"].as<string>();
							}
						}
						else {
							std::cout << "log config error: appender type is invalid, " << a
								<< std::endl;
							continue;
						}

						ld.appenders.push_back(lad);
					}
				}
				return ld;
			}
		};

		template<>
		class LeixiCast<LogDefine, string> {
		public:
			string operator()(const LogDefine& i) {
				YAML::Node n(YAML::NodeType::Map);
				n["name"] = i.name;
				if (i.level != LogLevel::UNKNOW) {
					n["level"] = LogLevel::toString(i.level);
				}
				if (!i.formatter.empty()) {
					n["formatter"] = i.formatter;
				}
				
				for (auto& a : i.appenders) {
					YAML::Node na(YAML::NodeType::Map);
					if (a.type == 1) {
						na["type"] = "FileLogAppender";
						na["file"] = a.file;
					}
					else if (a.type == 2) {
						na["type"] = "StdoutLogAppender";
					}
					if (a.level != LogLevel::UNKNOW) {
						na["level"] = LogLevel::toString(a.level);
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

	ConfigVar<set<LogDefine> >::sptr g_log_defines = Config::lookup("logs", set<LogDefine>(), "logs config");

	struct LogIniter {
		LogIniter() {
			g_log_defines->addLister(0xF1E231, [](const set<LogDefine>& old_value
				, const set<LogDefine>& new_value) {
					_LOG_INFO(_LOG_ROOT()) << "on logger conf changed";
					for (auto & i : new_value) {
						auto it = old_value.find(i);
						Logger::ptr logger;
						if (it != old_value.end() && i == *it)
							continue;
						logger = _LOG_NAME(i.name);

						logger->setLevel(i.level);
						if (!i.formatter.empty()) {
							logger->setFormatter(i.formatter);
						}
						logger->clearAppenders();
						for (auto & a : i.appenders) {
							LogAppender::ptr ap;
							if (a.type == 1) {
								ap.reset(new FileLogAppender(a.file));
							}
							if (a.type == 2) {
								ap.reset(new StdoutLogAppender);
							}
							if (a.level == LogLevel::UNKNOW)
								ap->setLevel(i.level);
							else 
								ap->setLevel(a.level);
							if (!a.formatter.empty()) {
								LogFormatter::ptr fmt(new LogFormatter(a.formatter));
								if(!fmt->isError())
									ap->setFormatter(fmt);
								else {
									std::cout << "logger name=" << i.name 
										<< " appender type=" << a.type
										<< " formatter=" << a.formatter 
										<< " is invaild" << std::endl;
								}
							}
							logger->addAppender(ap);
						}
					}
					for (auto & i : old_value) {
						auto it = new_value.find(i);
						if (it == new_value.end()) {
							// 删除
							auto logger = _LOG_NAME(i.name);
							logger->setLevel((LogLevel::Level)128);
							logger->clearAppenders();
						}
					}
				});
		}
	};

	static LogIniter __log_init;

	void LoggerManager::init() {

	}

	string LoggerManager::toYamlString()
	{
		YAML::Node node;
		for (auto & i : _loggers) {
			node.push_back(YAML::Load(i.second->toYamlString()));
		}
		stringstream ss;
		ss << node;
		return ss.str();
	}

	LoggerManager::LoggerManager() {
		_main = Logger::ptr(new Logger);
		_main->addAppender(LogAppender::ptr(new StdoutLogAppender));
		_loggers[_main->_name] = _main;
		init();
	}

	Logger::ptr LoggerManager::getLogger(const string & name)
	{
		auto it = _loggers.find(name);
		if (_loggers.end() != it)
			return it->second;
		Logger::ptr logger(new Logger(name));
		logger->_root = _main;
		return _loggers[name] = logger;
	}

	shared_ptr<LoggerManager> LoggerManager::getInstance()
	{
		static shared_ptr<LoggerManager> instance = shared_ptr<LoggerManager>(new LoggerManager());
		return instance;
	}


	void LogAppender::setFormatter(LogFormatter::ptr val)
	{
		_formatter = val; 
		_hasFormatter = _formatter ? true : false;
	}

}