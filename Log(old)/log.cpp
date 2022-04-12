#include "log.h"
#include <stdio.h>  
#include <stdarg.h>  

LogEvent::LogEvent(shared_ptr<Logger> logger, LogLevel::Level level, const char * file, int32_t line, uint32_t elapse, uint32_t threadId, uint32_t fiberId, uint64_t time, const string & threadName){
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
}

void Logger::log(LogLevel::Level level, LogEvent::ptr event){
	if (level < this->_level) return;
	for (auto it : _appenders)
		it->log(shared_from_this(), level, event);
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

void Logger::addAppender(LogAppender::ptr appender){
	if (!appender->getFormatter())
		appender->setFormatter(_formatter);
	_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
	for(auto it = _appenders.begin(); it != _appenders.end(); ++it)
		if (*it == appender) {
			_appenders.erase(it);
			break;
		}
}

/* ----------------------------end [Logger] end-------------------------------- */

/* ----------------------------begin [StdoutLogAppender] begin-------------------------------- */

void StdoutLogAppender::log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
	if (level < this->_level) return;
	std::cout << _formatter->format(logger, level, event) << std::endl;
}

/* ----------------------------end [StdoutLogAppender] end-------------------------------- */

/* ----------------------------begin [FileLogAppender] begin-------------------------------- */

FileLogAppender::FileLogAppender(const string & filename) : _filename(filename){
}

void FileLogAppender::log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
	if (level < this->_level) return;
	_filestream << _formatter->format(logger, level, event) << std::endl;
}

bool FileLogAppender::reopen(){
	if (_filestream) _filestream.close();
	_filestream.open(_filename);
	return !_filestream;
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
		if (s_format_items.end() == it) 
			_items.push_back(FormatItem::ptr(new StringFormatItem("<<error format %" + std::get<0>(i) + ">>")));
		else
			_items.push_back(it->second(std::get<1>(i)));

		//std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
	}
}

string LogFormatter::format(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
	std::stringstream os;
	for (auto i : _items)
		i->format(os, logger, level, event);
	return os.str();
}
/* ----------------------------end [LogFormatter] end-------------------------------- */

LoggerManager::LoggerManager(){
	_main = Logger::ptr(new Logger);
	_main->addAppender(LogAppender::ptr(new StdoutLogAppender));
}

Logger::ptr LoggerManager::getLogger(const string & name)
{
	auto it = _loggers.find(name);
	return _loggers.end() == it ? _main : it->second;
}

shared_ptr<LoggerManager> LoggerManager::getInstance()
{
	static shared_ptr<LoggerManager> instance = shared_ptr<LoggerManager>(new LoggerManager());
	return instance;
}


