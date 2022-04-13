#include <list>
#include "../include/config.h"

using std::pair;
using std::list;

namespace configx {
	Config::ConfigVarMap Config::s_data;

	static void listAllMember(const string & prefix, const YAML::Node & node, list<pair<string, YAML::Node>> & output) {
		if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") != string::npos) {
			_LOG_ERROR(_LOG_ROOT()) << "Config invaild name: " << prefix << " : " << node;
			throw std::invalid_argument(prefix);
		}
		output.push_back(std::make_pair(prefix, node));
		if (node.IsMap()) {
			for (auto it = node.begin(); it != node.end(); ++it) {
				listAllMember(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(), it->second, output);
			}
		}
	}

	void Config::loadFromYaml(const YAML::Node & root){
		list<pair<string, YAML::Node >> nodes;
		listAllMember("", root, nodes);
		for (auto & i : nodes) {
			string key = i.first;
			if (i.first.empty()) continue;
			ConfigVarBase::sptr var = lookupBase(i.first);
			if (var) {
				if (i.second.IsScalar()) {
					var->fromString(i.second.Scalar());
				}
				else {
					stringstream ss;
					ss << i.second;
					var->fromString(ss.str());
				}
			}
		}
	}
	ConfigVarBase::sptr Config::lookupBase(const string & name)
	{
		auto it = s_data.find(name);
		return it == s_data.end() ? nullptr : it->second;
	}
}