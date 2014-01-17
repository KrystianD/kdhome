#include "Program.h"

#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

// #include <LedUsbDevice.h>
// #include "SensorUsbDevice.h"
// #include "ExpanderUsbDevice.h"
#include "MyLogger.h"
#include "UdpServer.h"
#include "Timer.h"
#include "Controller.h"

// #include "stdafx.h"

// #include <boost/interprocess/ipc/message_queue.hpp>
// using namespace boost::interprocess;
using namespace std;

Program::Program() : m_logger(0)
{
	m_controller = new Controller();
}
Program::~Program()
{
	delete m_controller;
}
void Program::run()
{
	// m_db.connect();

	// UsbDevice::setLogger(logger);
	// UsbDevice::setLibusbContext(ctx);
	// SimpleUsbDevice::setLogger(logger);
	// SimpleUsbDevice::setLibusbContext(ctx);

	// LedUsbDevice usbLed("led_driver", 0x16c1, 0x05db);
	// usbLed.onEvent.connect(boost::bind(&Program::enqueueEvent, this, _1));

	// SensorUsbDevice usbSensor("sensor", 0x16c0, 0x05dd);
	// usbSensor.onEvent.connect(boost::bind(&Program::enqueueEvent, this, _1));

	// ExpanderUsbDevice usbExpander("expander", 0x16c1, 0x05df);
	// usbExpander.onEvent.connect(boost::bind(&Program::enqueueEvent, this, _1));
	
	// m_controller.registerTemperatureProvider(&usbLed, 0);
	// m_controller.registerTemperatureProvider(&usbSensor, 10);

	// m_controller.registerInputProvider(&usbExpander, 0);
	// m_controller.registerInputProvider(&usbLed, 10);

	// m_controller.registerOutputProvider(&usbExpander, 0);

	m_controller->setLogger(m_logger);
	m_controller->init();
	// m_controller.enableInputsChecking();

	// m_cs.setLogger(logger);
	// m_cs.createListener();
	// // m_cs.onCommand.connect(boost::bind(&Program::onExternalCommand, this, _1, _2));
	// m_csRemote.setLogger(logger);
	// m_csRemote.setPassword("kdhh");
	// m_csRemote.createListener();
	// // m_csRemote.onCommand.connect(boost::bind(&Program::onExternalCommand, this, _1, _2));

	Timer idleTimer(0, 100 * 1000);
	Timer kbdTimer(1);
	Timer testTimer(0, 500 * 1000);

	UdpServer srv;

	for (;;)
	{
		m_controller->execute();
		// m_cs.process();
		// m_csRemote.process();
		
		// UsbEvent *event = 0;
		// do
		// {
			// m_eventsMutex.lock();
			// if (!m_events.empty())
			// {
				// event = m_events.front();
				// m_events.pop();
			// }
			// else
			// {
				// event = 0;
			// }
			// m_eventsMutex.unlock();

			// if (event)
			// {
				// logger->logInfo(boost::format("New event - %1%") % event->toString());
				// // m_controller.onEvent(event);
				// delete event;
			// }
		// } while (event);

		// if (idleTimer.process())
		// {
			// int curIdleTime = getXIdleTime();
			// if (curIdleTime < lastIdleTime)
			// {
				// m_controller.getUserActivity().reset();
				// m_controller.getTotalActivity().reset();
			// }
			// lastIdleTime = curIdleTime;
		// }

		// if (testTimer.process())
		// {
		// printf("%d\n", m_keys[125]);
		// }

		usleep(5 * 1000);
	}
}

// void Program::enqueueEvent(UsbEvent* event)
// {
	// m_eventsMutex.lock();
	// m_events.push(event);
	// m_eventsMutex.unlock();
	// m_eventsCond.notify_one();
// }
// bool Program::onExternalCommand(const std::vector<std::string>& parts, std::string& res)
// {
	// return true;
	// // return m_controller.onExternalCommand(parts, res);
// }
