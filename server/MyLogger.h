#ifndef __MYLOGGER_H__
#define __MYLOGGER_H__

#include "Logger.h"
#include "format.h"

class MyLogger : public Logger
{
  public:
		using Logger::log;
		using Logger::logError;
		using Logger::logWarning;
		using Logger::logInfo;

    void log (LogType type, fmt::BasicFormatter<char>& _fmt) { logClass (type, 0, fmt::str (_fmt)); }
    void logError (fmt::BasicFormatter<char>& _fmt) { logClass (Error, 0, fmt::str (_fmt)); }
    void logWarning (fmt::BasicFormatter<char>& _fmt) { logClass (Warning, 0, fmt::str (_fmt)); }
    void logInfo (fmt::BasicFormatter<char>& _fmt) { logClass (Info, 0, fmt::str (_fmt)); }
};

#endif
