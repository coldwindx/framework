#pragma once

#include <unordered_map>
#include "config_define.h"
#include "config_var.h"

using std::pair;
using std::unordered_map;
using namespace logx;

__CONFIGX_BEGIN__


class Config {
public:
	typedef map<string, ConfigVarBase::sptr> ConfigVarMap;

	template<typename T>
	static typename ConfigVar<T>::sptr lookup(const string & name, const T & value, const string & description = "");

	static void loadFromYaml(const YAML::Node & root);
protected:
	static void _transfrom(const string & prefix, const YAML::Node & node, list<pair<string, YAML::Node>> & output);
	
private:
	static ConfigVarMap& configVarMap();
	
};

template<typename T>
typename ConfigVar<T>::sptr Config::lookup(const string & name, const T & value, const string & description) {
	auto it = configVarMap().find(name);
	if (configVarMap().end() != it) {
		auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
		if (tmp) {
			_LOG_INFO(_LOG_ROOT()) << "Lookup name= " << name << " exists";
			return tmp;
		}
		_LOG_ERROR(_LOG_ROOT()) << "Lookup name= " << name
			<< " exists but type not " << typeid(T).name()
			<< ",real type is " << it->second->getTypeName()
			<< " " << it->second->toString();
		return nullptr;
	}

	if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") != string::npos) {
		_LOG_ERROR(_LOG_ROOT()) << "Lookup name invaild: " << name;
		throw std::invalid_argument(name);
	}
	typename ConfigVar<T>::sptr v(new ConfigVar<T>(name, value, description));
	configVarMap()[name] = v;
	return v;
}
__CONFIGX_END__