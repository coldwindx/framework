#include <iostream>
#include <time.h>
#include "log.h"
using namespace std;
int main()
{
	Logger::ptr logger(new Logger);
	logger->addAppender(LogAppender::ptr(new StdoutLogAppender));
	LogEvent::ptr event(new LogEvent(logger, LogLevel::DEBUG, __FILE__, __LINE__, 0, 1, 2, time(0)));
	logger->log(LogLevel::DEBUG, event);

	_LOG_DEBUG(logger) << "test debug!";
	_LOG_FMT_DEBUG(logger, "%s say %d", "alice", 20);

	auto i = LoggerManager::getInstance()->getLogger("xxx");
	_LOG_DEBUG(i) << "test debug xxx!";
	system("pause");
	return 0;
}