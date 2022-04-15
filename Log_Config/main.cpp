#include<iostream>
#include <log.h>
#include <config.h>
#include "include\log_config.h"

using namespace std;
using namespace logx;
using namespace configx;

void test_config() {
	//cout << LogManager::instance()->toYamlString() << endl;

	YAML::Node root = YAML::LoadFile("config.yaml");
	cout << "====================================" << endl;
	Config::loadFromYaml(root);
	//cout << LogManager::instance()->toYamlString() << endl;
	_LOG_INFO(_LOG_NAME("system")) << "hello, system!";
}
int main()
{
	test_config();
	system("pause");
	return 0;
}