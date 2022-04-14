#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <boost\lexical_cast.hpp>

using std::stringstream;
using std::string;
using std::vector;
using std::set;
using std::list;
using std::map;
using std::unordered_set;
using std::unordered_map;
using std::function;
using std::exception;

/* 通用类型转换模板 */
template<typename F, typename T>
class Convert {
public:
	T operator()(const F & v) {
		return boost::lexical_cast<T>(v);
	}
};

template<typename T>
class Convert<string, vector<T> > {
public:
	vector<T> operator()(const string & v) {
		YAML::Node node = YAML::Load(v);
		vector<T> vec;
		stringstream ss;
		for (size_t i = 0; i < node.size(); ++i) {
			ss.clear();
			ss << node[i];
			vec.push_back(Convert<string, T>()(ss.str()));
		}
		return vec;
	}
};


template<typename T>
class Convert<vector<T>, string> {
public:
	string operator()(const vector<T> & v) {
		YAML::Node node;
		for (auto & i : v)
			node.push_back(YAML::Load(Convert<T, string>()(i)));

		stringstream ss;
		ss << node;
		return ss.str();
	}
};

template<typename T>
class Convert<string, list<T> > {
public:
	list<T> operator()(const string & v) {
		YAML::Node node = YAML::Load(v);
		list<T> vec;
		stringstream ss;
		for (size_t i = 0; i < node.size(); ++i) {
			ss.clear();
			ss << node[i];
			vec.push_back(Convert<string, T>()(ss.str()));
		}
		return vec;
	}
};

template<typename T>
class Convert<list<T>, string> {
public:
	string operator()(const list<T> & v) {
		YAML::Node node;
		for (auto & i : v)
			node.push_back(YAML::Load(Convert<T, string>()(i)));

		stringstream ss;
		ss << node;
		return ss.str();
	}
};

template<typename T>
class Convert<string, set<T> > {
public:
	set<T> operator()(const string & v) {
		YAML::Node node = YAML::Load(v);
		set<T> vec;
		stringstream ss;
		for (size_t i = 0; i < node.size(); ++i) {
			ss.str("");
			ss << node[i];
			vec.insert(Convert<string, T>()(ss.str()));
		}
		return vec;
	}
};

template<typename T>
class Convert<set<T>, string> {
public:
	string operator()(const set<T> & v) {
		YAML::Node node;
		for (auto & i : v)
			node.push_back(YAML::Load(Convert<T, string>()(i)));


		stringstream ss;
		ss << node;
		return ss.str();
	}
};


template<typename T>
class Convert<string, unordered_set<T> > {
public:
	unordered_set<T> operator()(const string & v) {
		YAML::Node node = YAML::Load(v);
		unordered_set<T> vec;
		stringstream ss;
		for (size_t i = 0; i < node.size(); ++i) {
			ss.str("");
			ss << node[i];
			vec.insert(Convert<string, T>()(ss.str()));
		}
		return vec;
	}
};

template<typename T>
class Convert<unordered_set<T>, string> {
public:
	string operator()(const unordered_set<T> & v) {
		YAML::Node node;
		for (auto & i : v)
			node.push_back(YAML::Load(Convert<T, string>()(i)));

		stringstream ss;
		ss << node;
		return ss.str();
	}
};


template<typename T>
class Convert<string, map<string, T> > {
public:
	map<string, T> operator()(const string & v) {
		YAML::Node node = YAML::Load(v);
		map<string, T> mp;
		stringstream ss;
		for (auto it = node.begin(); it != node.end(); ++it) {
			ss.str("");
			ss << it->second;
			mp.insert(std::make_pair(it->first.Scalar(), Convert<std::string, T>()(ss.str())));
		}
		return mp;
	}
};

template<typename T>
class Convert<map<string, T>, string> {
public:
	string operator()(const map<string, T> & v) {
		YAML::Node node(YAML::NodeType::Map);
		for (auto & i : v)
			node[i.first] = YAML::Load(Convert<T, string>()(i.second));

		stringstream ss;
		ss << node;
		return ss.str();
	}
};


template<typename T>
class Convert<string, unordered_map<string, T> > {
public:
	unordered_map<string, T> operator()(const string & v) {
		YAML::Node node = YAML::Load(v);
		unordered_map<string, T> mp;
		stringstream ss;
		for (auto it = node.begin(); it != node.end(); ++it) {
			ss.str("");
			ss << it->second;
			mp.insert(std::make_pair(it->first.Scalar(), Convert<std::string, T>()(ss.str())));
		}
		return mp;
	}
};

template<typename T>
class Convert<unordered_map<string, T>, string> {
public:
	string operator()(const unordered_map<string, T> & v) {
		YAML::Node node(YAML::NodeType::Map);
		for (auto & i : v)
			node[i.first] = YAML::Load(Convert<T, string>()(i.second));

		stringstream ss;
		ss << node;
		return ss.str();
	}
};
