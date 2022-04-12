#pragma once

#include <fstream>
#include "log_formatter.h"

using std::ofstream;

namespace logx {
	/* ��־��ӡ�� */
	class LogAppender {
	public:
		typedef std::shared_ptr<LogAppender> sptr;

		virtual ~LogAppender() {}

		virtual void log(LogEvent::sptr event) = 0;

		void setFormatter(LogFormatter::sptr val) { _formatter = val; }
		LogFormatter::sptr getFormatter() const { return _formatter; }
	protected:
		LogLevel::Level _level = LogLevel::DEBUG;
		LogFormatter::sptr _formatter;
	};


	/* �������׼��������� */
	class StdoutLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<StdoutLogAppender> sptr;

		virtual void log(LogEvent::sptr event) override;
	};

	/* ������ļ�������� */
	class FileLogAppender : public LogAppender {
	public:
		typedef std::shared_ptr<StdoutLogAppender> sptr;

		FileLogAppender(const string & filename);

		virtual void log(LogEvent::sptr event) override;

		bool reopen();
	private:
		string _filename;
		ofstream _filestream;
	};

}