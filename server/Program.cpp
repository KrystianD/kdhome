#include "Program.h"

#include <algorithm>
#include <fcntl.h>
#include <unistd.h>

#include "MyLogger.h"
#include "UdpServer.h"
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
	srand(time(0));

	m_controller->setLogger(m_logger);
	if(!m_controller->init())
	{
		m_logger->logError("Unable to init controller");
		return;
	}

	m_controller->run();
}
