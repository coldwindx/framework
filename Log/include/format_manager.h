#pragma once

#include <map>
#include <functional>
#include "format.h"

using std::map;
using std::function;

namespace logx {
	class FormatManager {
	public:
		typedef shared_ptr<FormatManager> sptr;

		static FormatManager::sptr instance() {
			static FormatManager::sptr p(new FormatManager());
			return p;
		}

		void login(const string & name, function<Format::sptr(const string & fmt)> func) {
			_formats[name] = func;
		}
		Format::sptr get(const string & name, const string & fmt = "") {
			return _formats[name](fmt);
		}
		bool has(const string & name) {
			return 0 < _formats.count(name);
		}
	private:
		map<string, function<Format::sptr(const string & fmt)>> _formats;


		FormatManager() {
			_formats["m"] = [](const string & fmt) { return Format::sptr(new MessageFormat(fmt)); };
			_formats["p"] = [](const string & fmt) { return Format::sptr(new LevelFormat(fmt)); };
			_formats["r"] = [](const string & fmt) { return Format::sptr(new ElapseFormat(fmt)); };
			_formats["c"] = [](const string & fmt) { return Format::sptr(new LoggerNameFormat(fmt)); };
			_formats["t"] = [](const string & fmt) { return Format::sptr(new ThreadIdFormat(fmt)); };
			_formats["n"] = [](const string & fmt) { return Format::sptr(new EnterFormat(fmt)); };
			_formats["d"] = [](const string & fmt) { return Format::sptr(new DateTimeFormat(fmt)); };
			_formats["f"] = [](const string & fmt) { return Format::sptr(new FilenameFormat(fmt)); };
			_formats["l"] = [](const string & fmt) { return Format::sptr(new LineFormat(fmt)); };
			_formats["T"] = [](const string & fmt) { return Format::sptr(new TabFormat(fmt)); };
			_formats["F"] = [](const string & fmt) { return Format::sptr(new FiberIdFormat(fmt)); };
		}
	};
}