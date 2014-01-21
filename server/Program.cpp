#include "Program.h"

#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

#include "MyLogger.h"
#include "UdpServer.h"
#include "Timer.h"
#include "Controller.h"

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
	m_controller->setLogger(m_logger);
	m_controller->init();

	for (;;)
	{
		m_controller->execute();
		usleep(5 * 1000);
	}
}
