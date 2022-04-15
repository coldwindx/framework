#pragma once

#include <sstream>
#include "config_define.h"
#include "convert.h"

using std::stringstream;
using namespace logx;

__CONFIGX_BEGIN__

class ConfigVarBase {
public:
	typedef shared_ptr<ConfigVarBase> sptr;

	ConfigVarBase(const string & name, const string & description = "") : _name(name), _description(description) {}
	virtual ~ConfigVarBase() {}

	const string & getName() const { return _name; }
	const string & getDecription() const { return _description; }

	virtual string getTypeName() const = 0;
	virtual string toString() = 0;
	virtual bool fromString(const string & val) = 0;
private:
	string _name;
	string _description;
};

template<typename T, typename From = Convert<string, T>, typename To = Convert<T, string> >
class ConfigVar : public ConfigVarBase {
public:
	typedef shared_ptr<ConfigVar> sptr;
	typedef function<void(const T & oldValue, const T & newValue)> onChange;

	ConfigVar(const string & name, const T & value, const string & description = "")
		: ConfigVarBase(name, description), _val(value) {}

	const T getValue() const { return _val; }
	// 通过 ConfigVarBase 继承
	virtual string getTypeName() const override { return typeid(T).name(); }
	virtual string toString() override;
	virtual bool fromString(const string & val) override;
	// 监听器相关函数
	void addLister(uint64_t key, onChange cb) {
		_onChanges[key] = cb;
	}
	void delLister(uint64_t key) {
		_onChanges.erase(key);
	}
	onChange getListener(uint64_t key) {
		auto it = _onChanges.find(key);
		return it == _onChanges.end() ? nullptr : it->second;
	}
	void clearListener() {
		_onChanges.clear();
	}
private:
	T _val;
	// 变更回调函数
	map<uint64_t, onChange> _onChanges;
};


template<typename T, typename From, typename To>
string ConfigVar<T, From, To>::toString() {
	try {
		return To()(_val);
	}
	catch (exception& e) {
		_LOG_ERROR(_LOG_ROOT()) << "ConfigVar::toString() exception" << e.what() << " convert: " << typeid(_val).name() << " to string";
	}
	return "";
}

template<typename T, typename From, typename To>
bool ConfigVar<T, From, To>::fromString(const string & val) {
	try {
		T v = From()(val);
		if (v == _val) return true;
		for (auto & i : _onChanges)
			i.second(_val, v);		// 触发监听函数
		_val = v;
		return true;
	}
	catch (exception & e) {
		_LOG_ERROR(_LOG_ROOT()) << "ConfigVar::toString() exception" << e.what() << " convert: string to" << typeid(_val).name();
	}
	return false;
}
__CONFIGX_END__