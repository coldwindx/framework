#include <iostream>
#include <time.h>
#include "log.h"
#include "config.h"

using namespace std;
using namespace sylar;

void test_1() {
	Logger::ptr logger(new Logger);
	logger->addAppender(LogAppender::ptr(new StdoutLogAppender));
	LogEvent::ptr event(new LogEvent(logger, LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0)));
	logger->log(LogLevel::DEBUG, event);

	_LOG_DEBUG(logger) << "test debug!";
	_LOG_FMT_DEBUG(logger, "%s say %d", "alice", 20);

	auto i = LoggerManager::getInstance()->getLogger("xxx");
	_LOG_DEBUG(i) << "test debug xxx!";
}
void test_config() {
	cout << LoggerManager::getInstance()->toYamlString() << endl;
	YAML::Node root = YAML::LoadFile("logs.yaml");
	cout << "====================================" << endl;
	Config::loadFromYaml(root);
	cout << LoggerManager::getInstance()->toYamlString() << endl;
}
int main()
{
	test_config();
	system("pause");
	return 0;
}