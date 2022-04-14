#include "../include/config.h"

using namespace logx;

__CONFIGX_BEGIN__

void Config::loadFromYaml(const YAML::Node & root) {
	list<pair<string, YAML::Node >> nodes;
	_transfrom("", root, nodes);
	for (auto & i : nodes) {
		string key = i.first;
		if (i.first.empty()) continue;
		auto it = configVarMap().find(i.first);
		if (configVarMap().end() != it) {
			if (i.second.IsScalar()) {
				it->second->fromString(i.second.Scalar());
			}
			else {
				stringstream ss;
				ss << i.second;
				it->second->fromString(ss.str());
			}
		}
	}
}

void Config::_transfrom(const string & prefix, const YAML::Node & node, list<pair<string, YAML::Node>>& output){
	if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") != string::npos) {
		_LOG_ERROR(_LOG_ROOT()) << "Config invaild name: " << prefix << " : " << node;
		throw std::invalid_argument(prefix);
	}
	output.push_back(std::make_pair(prefix, node));
	if (node.IsMap()) {
		for (auto it = node.begin(); it != node.end(); ++it) {
			_transfrom(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
		}
	}
}

Config::ConfigVarMap& Config::configVarMap(){
	static ConfigVarMap _configVarMap;
	return _configVarMap;
}

__CONFIGX_END__