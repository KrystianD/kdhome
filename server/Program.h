#ifndef __PROGRAM_H__
#define __PROGRAM_H__

class MyLogger;
class Controller;
// #include <UsbEvent.h>
// #include "Controller.h"
// #include "DB.h"
// #include "ControlServer.h"

#include <string>
#include <queue>
#include <vector>
#include <map>
// #include <boost/thread/mutex.hpp>
// #include <boost/thread/condition_variable.hpp>

class Program
{
public:
	Program();
	~Program();

	void setLogger(MyLogger* logger) { m_logger = logger; }
	void run();

	// void enqueueEvent(UsbEvent* event);
	// bool onExternalCommand(const std::vector<std::string>& parts, std::string& res);

	void logEvent(int category, const std::string& message) { } // m_db.logEvent(category, message); }

	Controller* getController() { return m_controller; }

private:
	MyLogger *m_logger;
	// std::queue<UsbEvent*> m_events;
	// boost::mutex m_eventsMutex, m_loopMutex;
	// boost::condition_variable m_eventsCond;

	Controller *m_controller;
	// DB m_db;
	// ControlServer m_cs, m_csRemote;
};

#endif
