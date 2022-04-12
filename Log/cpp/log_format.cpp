#include <tuple>
#include <map>
#include <functional>
#include "../include/log_formatter.h"
#include "../include/log_manager.h"
#include "../include/singleton.h"
#include "../include/format_manager.h"

using std::tuple;
using std::map;
using std::function;

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
			//std::cout << "pattern parse error: " << _pattern << " - " << _pattern.substr(i) << std::endl;
			_error = true;
			vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
		}
	}
	if (!nstr.empty())
		vec.push_back(std::make_tuple(nstr, string(), 0));

	FormatManager::sptr manager = SingletonPtr<FormatManager>::instance();
	for (auto & i : vec) {
		if (0 == std::get<2>(i)) {
			_formats.push_back(Format::sptr(new StringFormat(std::get<0>(i))));
			continue;
		}
		if(manager->has(std::get<0>(i)))
			_formats.push_back(manager->get(std::get<0>(i), std::get<1>(i)));
		else
			_formats.push_back(Format::sptr(new StringFormat("<<error format %" + std::get<0>(i) + ">>")));
	}
}

string LogFormatter::format(LogEvent::sptr event)
{
	std::stringstream os;
	for (auto i : _formats)
		i->format(os, event);
	return os.str();
}