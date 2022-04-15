#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdint.h>
#include <memory>
#include <tuple>
#include <list>
#include <vector>
#include <map>
#include <functional>
#include "xtime.h"
#include "util.h"

using std::stringstream;
using std::string;
using std::pair;
using std::tuple;
using std::list;
using std::vector;
using std::map;
using std::function;
using std::shared_ptr;

namespace sylar {

#define _LOG_LEVEL(logger, level) \
	if(logger->getLevel() <= level) \
		LogEventWrap(LogEvent::ptr(new LogEvent(logger, level, __FILE__, __LINE__, 0, GetThreadId(), GetFiberId(), time(0), ""))).getSS()
#define _LOG_DEBUG(logger) _LOG_LEVEL(logger, LogLevel::DEBUG)
#define _LOG_INFO(logger) _LOG_LEVEL(logger, LogLevel::INFO)
#define _LOG_WARN(logger) _LOG_LEVEL(logger, LogLevel::WARN)
#define _LOG_ERROR(logger) _LOG_LEVEL(logger, LogLevel::ERROR)
#define _LOG_FATAL(logger) _LOG_LEVEL(logger, LogLevel::FATAL)

#define _LOG_FMT_LEVEL(logger, level, fmt, ...) \
	if(logger->getLevel() <= level) \
		LogEventWrap(LogEvent::ptr(new LogEvent(logger, level, __FILE__, __LINE__, 0, GetThreadId(), GetFiberId(), time(0), ""))).getEvent()->format(fmt, __VA_ARGS__)
#define _LOG_FMT_DEBUG(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::DEBUG, fmt, __VA_ARGS__)
#define _LOG_FMT_INFO(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::INFO, fmt, __VA_ARGS__)
#define _LOG_FMT_WARN(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::WARN, fmt, __VA_ARGS__)
#define _LOG_FMT_ERROR(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::ERROR, fmt, __VA_ARGS__)
#define _LOG_FMT_FATAL(logger, fmt, ...) _LOG_FMT_LEVEL(logger, LogLevel::FATAL, fmt, __VA_ARGS__)

#define _LOG_ROOT() LoggerManager::getInstance()->getRoot()
#define _LOG_NAME(name) LoggerManager::getInstance()->getLogger(name)

	class Logger;
	class LogFormatter;
	class LoggerManager;
	/* 日志级别 */
	class LogLevel {
	public:
		enum Level { UNKNOW = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4, FATAL = 5 };
		static const char* toString(LogLevel::Level level) {
			switch (level) {
				case DEBUG: return "debug";
				case INFO: return "info";
				case WARN: return "warn";
				case ERROR: return "error";
				case FATAL: return "fatal";
				default: return "unknow";
			}		
		}

		static LogLevel::Level fromString(const string & name) {
			if (name == "debug") return DEBUG;
			if (name == "info") return INFO;
			if (name == "warn") return WARN;
			if (name == "error") return ERROR;
			if (name == "fatal") return FATAL;
			return UNKNOW;
		}
	};

	/* 日志事件 */
	class LogEvent {
	public:
		typedef std::shared_ptr<LogEvent> ptr;

		LogEvent(shared_ptr<Logger> logger, LogLevel::Level level, const char * file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time, const string& threadName = "");

		const char* getFile() const { return _file; }
		int32_t getLine() const { return _line; }
		uint32_t getThreadId() const { return _threadId; }
		//void setThreadId(uint32_t id) { this->_threadId = id; }
		string getThreadName() const { return _threadName; }
		uint32_t getFiberId() const { return _fiberId; }
		uint32_t getTime() const { return _time; }
		uint32_t getElapse() const { return _elapse; }
		shared_ptr<Logger> getLogger() const { return _logger; }
		LogLevel::Level getLevel() const { return _level; }
		string getContent() const { return _ss.str(); }
		stringstream& getSS() { return _ss; }
		void format(const char * fmt, ...);
	private:
		const char * _file = nullptr;		// 文件名
		int32_t _line = 0;					// 行号
		uint32_t _threadId = 0;				// 线程ID
		string _threadName;
		uint32_t _fiberId = 0;				// 协程ID
		uint32_t _time = 0;					// 时间戳
		uint32_t _elapse;					// 程序启动至此的时间

		shared_ptr<Logger> _logger;
		LogLevel::Level _level;
		stringstream _ss;
	};

	/* 日志格式器 */
	class LogFormatter {
	public:
		typedef std::shared_ptr<LogFormatter> ptr;
		LogFormatter(const string & pattern);

		void init();
		string format(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
		bool isError() const { return _error; }
		string getPattern() const { return _pattern; }

		class FormatItem {
		public:
			typedef std::shared_ptr<FormatItem> ptr;
			FormatItem(const string & fmt = "") {}
			virtual ~FormatItem() {}

			virtual void format(std::ostream& os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
		};
	private:
		string _pattern;
		vector<FormatItem::ptr> _items;
		bool _error = false;
	};

	/* 日志打印器 */
	class LogAppender {
		friend class Logger;
	public:
		typedef std::shared_ptr<LogAppender> ptr;

		virtual ~LogAppender() {}

		virtual void log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

		void setFormatter(LogFormatter::ptr val);
		LogFormatter::ptr getFormatter() const { return _formatter; }
		void setLevel(LogLevel::Level level) { _level = level; }

		virtual string toYamlString() = 0;
	protected:
		LogLevel::Level _level = LogLevel::DEBUG;
		bool _hasFormatter = false;
		LogFormatter::ptr _formatter;
	};


	class Logger : public std::enable_shared_from_this<Logger> {
		friend class LoggerManager;
	public:
		typedef std::shared_ptr<Logger> ptr;

		Logger(const string & filename = "root");

		void log(LogLevel::Level level, LogEvent::ptr event);
		void debug(LogEvent::ptr event);
		void info(LogEvent::ptr event);
		void warn(LogEvent::ptr event);
		void error(LogEvent::ptr event);
		void fatal(LogEvent::ptr event);

		void addAppender(LogAppender::ptr appender);
		void delAppender(LogAppender::ptr appender);
		void clearAppenders();

		LogLevel::Level getLevel() const { return _level; }
		void setLevel(LogLevel::Level level) { _level = level; }
		string getName() const { return _name; }

		void setFormatter(LogFormatter::ptr val);
		void setFormatter(const string& val);
		LogFormatter::ptr getFormater()const { return _formatter; }

		string toYamlString();
	private:
		string _name;
		LogLevel::Level _level = LogLevel::DEBUG;
		list<LogAppender::ptr> _appenders;
		LogFormatter::ptr _formatter;

		Logger::ptr _root;
	};

	class LogEventWrap {
	public:
		LogEventWrap(LogEvent::ptr e) : _event(e) {}
		~LogEventWrap() {
			_event->getLogger()->log(_event->getLevel(), _event);
		}

		LogEvent::ptr getEvent() const { return _event; }
		stringstream& getSS() { return _event->getSS(); }
	private:
		LogEvent::ptr _event;
	};


	class LoggerManager {
	public:
		Logger::ptr getLogger(const string & name);

		static shared_ptr<LoggerManager> getInstance();
		Logger::ptr getRoot() const { return _main; }

		void init();
		string toYamlString();
	private:
		LoggerManager();

		map<string, Logger::ptr> _loggers;
		Logger::ptr _main;
	};


	/* 输出到标准流的输出器 */
	class StdoutLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<StdoutLogAppender> ptr;

		virtual void log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

		// 通过 LogAppender 继承
		virtual string toYamlString() override;
	};

	/* 输出到文件的输出器 */
	class FileLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<StdoutLogAppender> ptr;

		FileLogAppender(const string & filename);

		virtual void log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

		bool reopen();
		// 通过 LogAppender 继承
		virtual string toYamlString() override;
	private:
		string _filename;
		std::ofstream _filestream;
	};


	class MessageFormatItem : public LogFormatter::FormatItem {
	public:
		MessageFormatItem(const string & fmt = "") {}

		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getContent();
		}
	};

	class LevelFormatItem : public LogFormatter::FormatItem {
	public:
		LevelFormatItem(const string & fmt = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << LogLevel::toString(level);
		}
	};

	class ElapseFormatItem : public LogFormatter::FormatItem {
	public:
		ElapseFormatItem(const string & fmt = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getElapse();
		}
	};

	class LoggerFormatItem : public LogFormatter::FormatItem {
	public:
		LoggerFormatItem(const string & fmt = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getLogger()->getName();
		}
	};

	class ThreadIdFormatItem : public LogFormatter::FormatItem {
	public:
		ThreadIdFormatItem(const string & fmt = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getThreadId();
		}
	};

	class FiberIdFormatItem : public LogFormatter::FormatItem {
	public:
		FiberIdFormatItem(const string & fmt = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getFiberId();
		}
	};

	class DateTimeFormatItem : public LogFormatter::FormatItem {
	public:
		DateTimeFormatItem(const string & format) {
			_format = format.empty() ? "%Y-%m-%d %H:%M:%S" : format;
		}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			time_t time = event->getTime();
			struct tm tm;
			char buf[64];
			xlocaltime(time, tm);
			strftime(buf, sizeof(buf), _format.c_str(), &tm);
			os << buf;
		}
	private:
		string _format;
	};

	class FilenameFormatItem : public LogFormatter::FormatItem {
	public:
		FilenameFormatItem(const string & fmt = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getFile();
		}
	};

	class LineFormatItem : public LogFormatter::FormatItem {
	public:
		LineFormatItem(const string & fmt = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getLine();
		}
	};

	class EnterFormatItem : public LogFormatter::FormatItem {
	public:
		EnterFormatItem(const string & fmt = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << event->getLine();
		}
	};

	class StringFormatItem : public LogFormatter::FormatItem {
	public:
		StringFormatItem(const string & str = "") : _string(str) {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << this->_string;
		}
	private:
		string _string;
	};


	class TabFormatItem : public LogFormatter::FormatItem {
	public:
		TabFormatItem(const string & str = "") {}
		// 通过 FormatItem 继承
		virtual void format(std::ostream & os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
			os << "\t";
		}
	};
}