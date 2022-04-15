#include <iostream>
#include "include\log_appender.h"

namespace logx {
	void StdoutLogAppender::log(LogEvent::sptr event) {
		if (event->level < this->_level) return;
		std::cout << _formatter->format(event) << std::endl;
	}

	FileLogAppender::FileLogAppender(const string & filename) : _filename(filename) {
	}

	void FileLogAppender::log(LogEvent::sptr event) {
		if (event->level < this->_level) return;
		reopen();
		_filestream << _formatter->format(event) << std::endl;
	}

	bool FileLogAppender::reopen() {
		if (_filestream) _filestream.close();
		_filestream.open(_filename, std::ios::app);
		return !_filestream;
	}
}