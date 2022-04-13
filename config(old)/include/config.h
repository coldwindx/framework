#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <boost\lexical_cast.hpp>
#include <log.h>					// 引用Log项目
#include <yaml-cpp\yaml.h>

#pragma comment(lib, "Log(old).lib")

using std::shared_ptr;
using std::string;
using std::map;
using std::list;
using std::set;
using std::unordered_set;
using std::unordered_map;
using std::function;
using std::exception;

namespace configx {
	class ConfigVarBase {
	public:
		typedef shared_ptr<ConfigVarBase> sptr;

		ConfigVarBase(const string & name, const string & description = "") : _name(name), _description(description) {}
		virtual ~ConfigVarBase() {}

		const string & getName() const { return _name; }
		const string & getScription() const { return _description; }
		virtual string getTypeName() const = 0;

		virtual string toString() = 0;
		virtual bool fromString(const string & val) = 0;
	private:
		string _name;
		string _description;
	};

	template<typename F, typename T>
	class LeixiCast {
	public:
		T operator()(const F & v) {
			return boost::lexical_cast<T>(v);
		}
	};

	template<typename T>
	class LeixiCast<string, vector<T> > {
	public:
		vector<T> operator()(const string & v) {
			YAML::Node node = YAML::Load(v);
			vector<T> vec;
			stringstream ss;
			for (size_t i = 0; i < node.size(); ++i) {
				ss.clear();
				ss << node[i];
				vec.push_back(LeixiCast<string, T>()(ss.str()));
			}
			return vec;
		}
	};


	template<typename T>
	class LeixiCast<vector<T>, string> {
	public:
		string operator()(const vector<T> & v) {
			YAML::Node node;
			for (auto & i : v) 
				node.push_back(YAML::Load(LeixiCast<T, string>()(i)));
		

			stringstream ss;
			ss << node;
			return ss.str();
		}
	};
	
	template<typename T>
	class LeixiCast<string, list<T> > {
	public:
		list<T> operator()(const string & v) {
			YAML::Node node = YAML::Load(v);
			list<T> vec;
			stringstream ss;
			for (size_t i = 0; i < node.size(); ++i) {
				ss.clear();
				ss << node[i];
				vec.push_back(LeixiCast<string, T>()(ss.str()));
			}
			return vec;
		}
	};

	template<typename T>
	class LeixiCast<list<T>, string> {
	public:
		string operator()(const list<T> & v) {
			YAML::Node node;
			for (auto & i : v)
				node.push_back(YAML::Load(LeixiCast<T, string>()(i)));


			stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<typename T>
	class LeixiCast<string, set<T> > {
	public:
		set<T> operator()(const string & v) {
			YAML::Node node = YAML::Load(v);
			set<T> vec;
			stringstream ss;
			for (size_t i = 0; i < node.size(); ++i) {
				ss.str("");
				ss << node[i];
				vec.insert(LeixiCast<string, T>()(ss.str()));
			}
			return vec;
		}
	};

	template<typename T>
	class LeixiCast<set<T>, string> {
	public:
		string operator()(const set<T> & v) {
			YAML::Node node;
			for (auto & i : v)
				node.push_back(YAML::Load(LeixiCast<T, string>()(i)));


			stringstream ss;
			ss << node;
			return ss.str();
		}
	};


	template<typename T>
	class LeixiCast<string, unordered_set<T> > {
	public:
		unordered_set<T> operator()(const string & v) {
			YAML::Node node = YAML::Load(v);
			unordered_set<T> vec;
			stringstream ss;
			for (size_t i = 0; i < node.size(); ++i) {
				ss.str("");
				ss << node[i];
				vec.insert(LeixiCast<string, T>()(ss.str()));
			}
			return vec;
		}
	};

	template<typename T>
	class LeixiCast<unordered_set<T>, string> {
	public:
		string operator()(const unordered_set<T> & v) {
			YAML::Node node;
			for (auto & i : v)
				node.push_back(YAML::Load(LeixiCast<T, string>()(i)));

			stringstream ss;
			ss << node;
			return ss.str();
		}
	};


	template<typename T>
	class LeixiCast<string, map<string, T> > {
	public:
		map<string, T> operator()(const string & v) {
			YAML::Node node = YAML::Load(v);
			map<string, T> mp;
			stringstream ss;
			for (auto it = node.begin(); it != node.end(); ++it) {
				ss.str("");
				ss << it->second;
				mp.insert(std::make_pair(it->first.Scalar(), LeixiCast<std::string, T>()(ss.str())));
			}
			return mp;
		}
	};

	template<typename T>
	class LeixiCast<map<string, T>, string> {
	public:
		string operator()(const map<string, T> & v) {
			YAML::Node node(YAML::NodeType::Map);
			for (auto & i : v)
				node[i.first] = YAML::Load(LeixiCast<T, string>()(i.second));

			stringstream ss;
			ss << node;
			return ss.str();
		}
	};


	template<typename T>
	class LeixiCast<string, unordered_map<string, T> > {
	public:
		unordered_map<string, T> operator()(const string & v) {
			YAML::Node node = YAML::Load(v);
			unordered_map<string, T> mp;
			stringstream ss;
			for (auto it = node.begin(); it != node.end(); ++it) {
				ss.str("");
				ss << it->second;
				mp.insert(std::make_pair(it->first.Scalar(), LeixiCast<std::string, T>()(ss.str())));
			}
			return mp;
		}
	};

	template<typename T>
	class LeixiCast<unordered_map<string, T>, string> {
	public:
		string operator()(const unordered_map<string, T> & v) {
			YAML::Node node(YAML::NodeType::Map);
			for (auto & i : v)
				node[i.first] = YAML::Load(LeixiCast<T, string>()(i.second));

			stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<typename T, typename From = LeixiCast<string, T>
						, typename To = LeixiCast<T, string> >
	class ConfigVar : public ConfigVarBase {
	public:
		typedef shared_ptr<ConfigVar> sptr;
		typedef function<void(const T & old_value, const T & new_value)> on_change_cb;

		ConfigVar(const string & name, const T & value, const string & description = "")
			: ConfigVarBase(name, description), _val(value) {}

		const T getValue() const { return _val;  }
		void setValue(const T & v) { 
			if (v == _val) return;
			for (auto & i : _cbs) {
				i.second(_val, v);		// 触发监听函数
			}
			_val = v;
		}
		virtual string getTypeName() const override { return typeid(T).name(); }

		// 通过 ConfigVarBase 继承
		virtual string toString() override {
			try {
				//return boost::lexical_cast<string>(_val);
				return To()(_val);
			}
			catch (exception& e) {
				_LOG_ERROR(_LOG_ROOT()) << "ConfigVar::toString() exception" << e.what() << " convert: " << typeid(_val).name() << " to string";
				
			}
			return "";
		}
		virtual bool fromString(const string & val) override {
			try {
				//_val = boost::lexical_cast<T>(val);
				setValue(From()(val));
			}
			catch (exception & e) {
				_LOG_ERROR(_LOG_ROOT()) << "ConfigVar::toString() exception" << e.what() << " convert: string to" << typeid(_val).name();
			}
			return false;
		}

		void addLister(uint64_t key, on_change_cb cb) {
			_cbs[key] = cb;
		}
		void delLister(uint64_t key) {
			_cbs.erase(key);
		}
		on_change_cb getListener(uint64_t key) {
			auto it = _cbs.find(key);
			return it == _cbs.end() ? nullptr : it->second;
		}
		void clearListener() {
			_cbs.clear();
		}
	private:
		T _val;
		// 变更回调函数
		map<uint64_t, on_change_cb> _cbs;
	};

	class Config {
	public:
		typedef unordered_map<string, ConfigVarBase::sptr> ConfigVarMap;

		template<typename T>
		static typename ConfigVar<T>::sptr lookup(const string & name, const T & value, const string & description = "") {
			auto it = s_data.find(name);
			if (s_data.end() != it) {
				auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
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
			s_data[name] = v;
			return v;
		}
/*
		template<typename T>
		static typename ConfigVar<T>::sptr lookup(const string & name) {
			auto it = s_data.find(name);
			if (s_data.end() == it)
				return nullptr;
			return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
		}
*/
		static void loadFromYaml(const YAML::Node & root);

		static ConfigVarBase::sptr lookupBase(const string & name);
	private:
		static ConfigVarMap s_data;
	};

	
}

