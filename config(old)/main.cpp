#include<iostream>
#include <vector>
#include <log.h>
#include <yaml-cpp\yaml.h>
#include "include\config.h"

#pragma comment(lib, "yaml-cppd.lib")

using namespace std;
using namespace configx;

// 自定义配置
ConfigVar<int>::sptr g_int_value_config = Config::lookup("system.int", 1, "system int");
ConfigVar<float>::sptr g_float_value_config = Config::lookup("system.int", 0.1f, "system int");
ConfigVar<vector<int>>::sptr g_int_vec_value_config = Config::lookup("system.int_vec", vector<int>{0,1,2}, "system int vec");
ConfigVar<list<int>>::sptr g_int_list_value_config = Config::lookup("system.int_list", list<int>{0, 1, 2}, "system int list");
ConfigVar<set<int>>::sptr g_int_set_value_config = Config::lookup("system.int_set", set<int>{0, 1, 2}, "system int set");
ConfigVar<unordered_set<int>>::sptr g_int_uset_value_config = Config::lookup("system.int_uset", unordered_set<int>{0, 1, 2}, "system int uset");
ConfigVar<map<string, int>>::sptr g_int_map_value_config = Config::lookup("system.int_map", map<string, int>{ {"k", 2}}, "system int map");




void print_yaml(const YAML::Node & node, int level) {
	if (node.IsScalar()) {
		_LOG_INFO(_LOG_ROOT()) << string(level * 4, ' ') << node.Scalar() << " - " << node.Type() << " - " << level;
	}
	else if(node.IsNull()){
		_LOG_INFO(_LOG_ROOT()) << string(level * 4, ' ') << "NULL - " << node.Type() << " - " << level;
	}
	else if (node.IsMap()) {
		for (auto it = node.begin(); it != node.end(); ++it) {
			_LOG_INFO(_LOG_ROOT()) << string(level * 4, ' ') << it->first <<  " - " << it->second.Type() << " - " << level;
			print_yaml(it->second, level + 1);
		}
	}
	else if (node.IsSequence()) {
		for (int i = 0, n = node.size(); i < n; ++i) {
			_LOG_INFO(_LOG_ROOT()) << string(level * 4, ' ') << i << " - " << node[i].Type() << " - " << level;
			print_yaml(node[i], level + 1);
		}
	}
}

void test_yaml() {

#define XX(g_var, name, prefix) \
	{	\
		auto & v = g_var->getValue();	\
		for(auto & i : v){ \
			_LOG_INFO(_LOG_ROOT()) << #prefix " " #name ": " << i;	\
		} \
		_LOG_INFO(_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
	}

#define XXM(g_var, name, prefix) \
	{	\
		auto & v = g_var->getValue();	\
		for(auto & i : v){ \
			_LOG_INFO(_LOG_ROOT()) << #prefix " " #name ": {" << i.first << " - " << i.second << "}";	\
		} \
		_LOG_INFO(_LOG_ROOT()) << #prefix " " #name " yaml: " << g_var->toString(); \
	}

	XX(g_int_set_value_config, int_set, before);
	XX(g_int_uset_value_config, int_uset, before);
	XXM(g_int_map_value_config, int_map, before);

	// 从文件获取配置
	YAML::Node root = YAML::LoadFile("config.yaml");
	Config::loadFromYaml(root);

	XX(g_int_set_value_config, int_set, after);
	XX(g_int_uset_value_config, int_uset, after);
	XXM(g_int_map_value_config, int_map, after);
	
}

class Person {
public:
	string _name = "";
	int _age = 0;
	bool _sex = 0;

	string toString() const {
		stringstream ss;
		ss << "[Person name=" << _name << " age=" << _age << " sex=" << _sex << "]";
		return ss.str();
	}

	bool operator==(const Person & oth) const {
		return _name == oth._name && _age == oth._age && _sex == oth._sex;
	}
};

namespace configx {

	template<>
	class LeixiCast<string, Person> {
	public:
		Person operator()(const string & v) {
			YAML::Node node = YAML::Load(v);
			Person p;
			p._name = node["name"].as<string>();
			p._age = node["age"].as<int>();
			p._sex = node["sex"].as<bool> ();
			return p;
		}
	};


	template<>
	class LeixiCast<Person, string> {
	public:
		string operator()(const Person & p) {
			YAML::Node node;
			node["name"] = p._name;
			node["age"] = p._age;
			node["sex"] = p._sex;
			
			stringstream ss;
			ss << node;
			return ss.str();
		}
	};

}

ConfigVar<Person>::sptr g_person_value_config = Config::lookup("system.class.person", Person(), "system person");



void test_class() {
	_LOG_INFO(_LOG_ROOT()) << g_person_value_config->getValue().toString() << " - " << g_person_value_config->toString();

	g_person_value_config->addLister(10, [](const Person & old_value, const Person & new_value) {
		_LOG_INFO(_LOG_ROOT()) << "old_value=" << old_value.toString()
			<< " new_value=" << new_value.toString();
	});
	// 从文件获取配置
	YAML::Node root = YAML::LoadFile("config.yaml");
	Config::loadFromYaml(root);

	_LOG_INFO(_LOG_ROOT()) << g_person_value_config->getValue().toString() << " - " << g_person_value_config->toString();

}

int main()
{
	//test_yaml();
	test_class();
	system("pause");
	return 0;
}