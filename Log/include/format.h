#pragma once

#include <memory>
#include <string>
#include "log_event.h"
#include "log_util.h"

using std::string;
using std::shared_ptr;

/* 日志信息格式器 */
class Format {
public:
	typedef shared_ptr<Format> sptr;

	Format(const string & fmt = "") {}
	virtual ~Format() {}

	virtual void format(std::ostream& os, LogEvent::sptr event) = 0;
};

/*--------------------------------消息体格式化--------------------------------------*/
class MessageFormat : public Format {
public:
	MessageFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os,LogEvent::sptr event) override {
		os << event->ss.str();
	}
};


class EnterFormat : public Format {
public:
	EnterFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << "\n";
	}
};

class StringFormat : public Format {
public:
	StringFormat(const string & str = "") : _string(str) {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << this->_string;
	}
private:
	string _string;
};


class TabFormat : public Format {
public:
	TabFormat(const string & str = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << "\t";
	}
};
/*--------------------------------日志信息格式化--------------------------------------*/
class LevelFormat : public Format {
public:
	LevelFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << LogLevel::getName(event->level);
	}
};

class LoggerNameFormat : public Format {
public:
	LoggerNameFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << event->loggerName;
	}
};


/*--------------------------------程序信息格式化--------------------------------------*/
class ElapseFormat : public Format {
public:
	ElapseFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << event->elapse;
	}
};


class LineFormat : public Format {
public:
	LineFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << event->line;
	}
};


/*--------------------------------线程信息格式化--------------------------------------*/
class ThreadIdFormat : public Format {
public:
	ThreadIdFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << std::hex << event->threadId;
	}
};

/*--------------------------------协程信息格式化--------------------------------------*/
class FiberIdFormat : public Format {
public:
	FiberIdFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << event->fiberId;
	}
};

/*--------------------------------时间信息格式化--------------------------------------*/
class DateTimeFormat : public Format {
public:
	DateTimeFormat(const string & format) {
		_format = format.empty() ? "%Y-%m-%d %H:%M:%S" : format;
	}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		time_t time = event->time;
		struct tm tm;
		char buf[64];
		LogUtil::xlocaltime(time, tm);
		strftime(buf, sizeof(buf), _format.c_str(), &tm);
		os << buf;
	}
private:
	string _format;
};

/*--------------------------------文件信息格式化--------------------------------------*/
class FilenameFormat : public Format {
public:
	FilenameFormat(const string & fmt = "") {}
	// 通过 Format 继承
	virtual void format(std::ostream & os, LogEvent::sptr event) override {
		os << event->filename;
	}
};
