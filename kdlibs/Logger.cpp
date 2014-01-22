#include "Logger.h"
using namespace std;

#include <string.h>
#include <stdio.h>
#include <cstdarg> // va_list
#include <algorithm> // find

using namespace std;

#define PARSE_ARGS(strObj,str) \
  va_list args; \
  va_start (args, str); \
  int len = vsnprintf (0, 0, str, args); \
  va_end (args); \
	char *msg = new char[len + 1]; \
  va_start (args, str); \
  vsnprintf (msg, len + 1, str, args); \
  va_end (args); \
  strObj = msg; \
  delete [] msg;

Logger::Logger ()
{
  pthread_mutex_init (&m_mutex, 0);
}
Logger::~Logger ()
{
  pthread_mutex_destroy (&m_mutex);
}

void Logger::log (LogType type, const char* str, ...)
{
	lock ();

  string msgStr;
  PARSE_ARGS (msgStr, str);
  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (type, 0, dateStr, msgStr);

  unlock ();
}
void Logger::logError (const char* str, ...)
{
	lock ();

  string msgStr;
  PARSE_ARGS (msgStr, str);
  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (Logger::Error, 0, dateStr, msgStr);

  unlock ();
}
void Logger::logWarning (const char* str, ...)
{
  lock ();

  string msgStr;
  PARSE_ARGS (msgStr, str);
  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (Logger::Warning, 0, dateStr, msgStr);

  unlock ();
}
void Logger::logInfo (const char* str, ...)
{
	lock ();

  string msgStr;
  PARSE_ARGS (msgStr, str);
  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (Logger::Info, 0, dateStr, msgStr);

  unlock ();
}
void Logger::logClass (LogType type, int msgClass, const string& str)
{
	lock ();

  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (type, msgClass, dateStr, str);

  unlock ();
}
void Logger::logClass (LogType type, int msgClass, const char* str, ...)
{
  lock ();

  string msgStr;
  PARSE_ARGS (msgStr, str);
  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (type, msgClass, dateStr, msgStr);

  unlock ();
}
void Logger::logErrorClass (int msgClass, const char* str, ...)
{
  lock ();

  string msgStr;
  PARSE_ARGS (msgStr, str);
  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (Logger::Error, msgClass, dateStr, msgStr);

  unlock ();
}
void Logger::logWarningClass (int msgClass, const char* str, ...)
{
  lock ();

  string msgStr;
  PARSE_ARGS (msgStr, str);
  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (Logger::Warning, msgClass, dateStr, msgStr);

  unlock ();
}
void Logger::logInfoClass (int msgClass, const char* str, ...)
{
  lock ();

  string msgStr;
  PARSE_ARGS (msgStr, str);
  string dateStr = getDate ();

  for (size_t i = 0; i < m_handlers.size (); i++)
    m_handlers[i]->log (Logger::Info, msgClass, dateStr, msgStr);

  unlock ();
}

string Logger::getDate ()
{
  time_t _time = time (0);
  char *date = asctime (localtime (&_time));
  *(strrchr (date, '\n')) = '\0';
	return date;
}
void Logger::lock ()
{
	pthread_mutex_lock(&m_mutex);
}
void Logger::unlock ()
{
	pthread_mutex_unlock(&m_mutex);
}

bool LoggerOutput::isClassAllowed (int msgClass)
{
	return std::find (m_disabledClasses.begin (), m_disabledClasses.end (), msgClass) == m_disabledClasses.end ();
}

#define COLOR_RED "\33[1;31m"
#define COLOR_YELLOW "\33[1;33m"
#define COLOR_GREEN "\33[1;32m"
#define COLOR_DEFAULT "\33[0m"

void ConsoleLoggerOutput::log (Logger::LogType type, int msgClass, const string& date, const string& message)
{
	if (!isClassAllowed (msgClass)) return;
  const char* typeStr;
  
  if (m_useColors)
  {
		if (type == Logger::Error)
			printf (COLOR_RED);
		else if (type == Logger::Warning)
			printf (COLOR_YELLOW);
	}
  
  printf ("[%s] ", date.c_str ());
  
  switch (type)
  {
    case Logger::Error:
			printf ("[error] ");
			break;
    case Logger::Warning:
			printf ("[warning] ");
			break;
    case Logger::Info:
			if (m_useColors)
				printf (COLOR_GREEN "[info]" COLOR_DEFAULT " ");
			else
				printf ("[info] ");
			break;
  }

  printf ("%s", message.c_str ());
	if (m_useColors)
		printf ("\33[0m");
  printf ("\n");
}

void FileLoggerOutput::log (Logger::LogType type, int msgClass, const string& date, const string& message)
{
	if (!isClassAllowed (msgClass)) return;
  const char* typeStr;
  switch (type)
  {
    case Logger::Error: typeStr = "error"; break;
    case Logger::Warning: typeStr = "warning"; break;
    case Logger::Info: typeStr = "info"; break;
  }

  FILE *logfile = fopen (m_file.c_str (), "a");
  if (!logfile) return;
  fprintf (logfile, "[%s] [%s] %s\n", date.c_str (), typeStr, message.c_str ());
  fclose (logfile);
}
