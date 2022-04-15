#pragma once

#include <vector>
#include "format.h"

using std::vector;

namespace logx {
	/* ��־��ʽ�� */
	class LogFormatter {
	public:
		typedef std::shared_ptr<LogFormatter> sptr;
		LogFormatter(const string & pattern);

		void init();
		string format(LogEvent::sptr event);
		bool isError()const { return _error; }
	private:
		bool _error = false;
		string _pattern;
		vector<Format::sptr> _formats;
	};
}