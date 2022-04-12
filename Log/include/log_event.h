#pragma once

#include <sstream>
#include "log_level.h"

using std::stringstream;

namespace logx {
	/* 日志事件 */
	struct LogEvent {
		typedef std::shared_ptr<LogEvent> sptr;

		const char * filename = nullptr;	// 文件名
		int32_t line = 0;					// 行号
		uint32_t threadId = 0;				// 线程ID
		uint32_t fiberId = 0;				// 协程ID
		uint32_t time = 0;					// 时间戳
		uint32_t elapse;					// 程序启动至此的时间
		const char * loggerName = nullptr;	// 日志名称
		LogLevel::Level level;				// 日志级别
		stringstream ss;					// 消息体

		LogEvent(const char * filename
			, int32_t line
			, LogLevel::Level level
			, uint32_t elapse
			, uint32_t threadId
			, uint32_t fiberId
			, uint64_t time) {
			this->filename = filename;
			this->line = line;
			this->level = level;
			this->elapse = elapse;
			this->threadId = threadId;
			this->fiberId = fiberId;
			this->time = time;
		}
	};
}