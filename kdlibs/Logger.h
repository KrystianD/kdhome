#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <cstdlib>
#include <string>
#include <vector>
#include <pthread.h>

// #include <boost/thread/mutex.hpp>

class LoggerOutput;
class Logger
{
  public:
    enum LogType { Error, Warning, Info };

    Logger ();
    ~Logger ();

    void log (LogType type, const char* str, ...);
    void logError (const char* str, ...);
    void logWarning (const char* str, ...);
    void logInfo (const char* str, ...);
    
#ifdef BOOST_FORMAT_CLASS_HPP
    void log (LogType type, boost::format fmt) { logClass (type, 0, boost::str (fmt)); }
    void logError (boost::format fmt) { logClass (Error, 0, boost::str (fmt)); }
    void logWarning (boost::format fmt) { logClass (Warning, 0, boost::str (fmt)); }
    void logInfo (boost::format fmt) { logClass (Info, 0, boost::str (fmt)); }
#endif

    void log (LogType type, const std::string& msg) { logClass (type, 0, msg); }
    void logError (const std::string& msg) { logClass (Error, 0, msg); }
    void logWarning (const std::string& msg) { logClass (Warning, 0, msg); }
    void logInfo (const std::string& msg) { logClass (Info, 0, msg); }

    void logClass (LogType type, int msgClass, const std::string& str);

    void logClass (LogType type, int msgClass, const char* str, ...);
    void logErrorClass (int msgClass, const char* str, ...);
    void logWarningClass (int msgClass, const char* str, ...);
    void logInfoClass (int msgClass, const char* str, ...);

    void addOutput (LoggerOutput* output) { m_handlers.push_back (output); }

  protected:
		std::string getDate ();

    std::vector<LoggerOutput*> m_handlers;

    pthread_mutex_t m_mutex;
    // boost::mutex m_boostMutex;
    
		// void lock () { m_boostMutex.lock (); }
		// void unlock () { m_boostMutex.unlock (); }
		void lock ();
		void unlock ();
};
class LoggerOutput
{
  public:
    virtual void log (Logger::LogType type, int msgClass, const std::string& date, const std::string& message) = 0;

		void disableClass (int msgClass) { m_disabledClasses.push_back (msgClass); }

	protected:
		bool isClassAllowed (int msgClass);

	private:
		std::vector<int> m_disabledClasses;
};
class ConsoleLoggerOutput : public LoggerOutput
{
  public:
		ConsoleLoggerOutput () : m_useColors (false) { }
  
		void enableColors () { m_useColors = true; }
		void disableColors () { m_useColors = false; }
  
    void log (Logger::LogType type, int msgClass, const std::string& date, const std::string& message);
    
	private:
		bool m_useColors;
};
class FileLoggerOutput : public LoggerOutput
{
  public:
    FileLoggerOutput (const std::string& file) : m_file (file) { }

    void log (Logger::LogType type, int msgClass, const std::string& date, const std::string& message);

  private:
    std::string m_file;
};
class ExitLoggerOutput : public LoggerOutput
{
  public:
    void log (Logger::LogType type, int msgClass, const std::string& date, const std::string& message) { if (type == Logger::Error) exit (1); }
};

#endif
