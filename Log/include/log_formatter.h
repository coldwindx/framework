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
	private:
		bool _error;
		string _pattern;
		vector<Format::sptr> _formats;
	};
}