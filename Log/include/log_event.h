#pragma once

#include <sstream>
#include "log_level.h"

using std::stringstream;

namespace logx {
	/* ��־�¼� */
	struct LogEvent {
		typedef std::shared_ptr<LogEvent> sptr;

		const char * filename = nullptr;	// �ļ���
		int32_t line = 0;					// �к�
		uint32_t threadId = 0;				// �߳�ID
		uint32_t fiberId = 0;				// Э��ID
		uint32_t time = 0;					// ʱ���
		uint32_t elapse;					// �����������˵�ʱ��
		const char * loggerName = nullptr;	// ��־����
		LogLevel::Level level;				// ��־����
		stringstream ss;					// ��Ϣ��

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