#include<iostream>
#include "include\singleton.h"
#include "include\log.h"

using namespace std;
int main()
{
	stringstream os;

	LogFormatter::sptr f = shared_ptr<LogFormatter>(new LogFormatter("%d [%p] %f:%l %m %n"));

	LogAppender::sptr a = shared_ptr<StdoutLogAppender>(new StdoutLogAppender);
	a->setFormatter(f);

	Logger::sptr l = shared_ptr<Logger>(new Logger);
	l->addAppender(a);

	SingletonPtr<LogManager>::instance()->login("this", l);

	Logger::sptr p = SingletonPtr<LogManager>::instance()->get("this");

	_LOG_DEBUG(p) << "hello";
	system("pause");
	return 0;
}