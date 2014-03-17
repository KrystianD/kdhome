#include "Program.h"

#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <poll.h>
#include <libusb.h>

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
	srand(time(0));

	m_controller->setLogger(m_logger);
	m_controller->init();

	for (;;)
	{
		m_controller->execute();
		// usleep(5 * 1000);

		const libusb_pollfd** libfds = libusb_get_pollfds(0);
		fd_set fdsin, fdsout;
		FD_ZERO(&fdsin);
		FD_ZERO(&fdsout);
		int max = 0;
		for (int i = 0; libfds[i]; i++)
		{
			if (libfds[i]->events & POLLIN)
				FD_SET(libfds[i]->fd, &fdsin);
			else if (libfds[i]->events & POLLOUT)
				FD_SET(libfds[i]->fd, &fdsout);
			if (libfds[i]->fd > max) max = libfds[i]->fd;
		}
		free(libfds);

		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 5 * 1000;
		int res = select(max + 1, &fdsin, &fdsout, 0, &tv);
		if (res)
		{
			int tim = libusb_get_next_timeout(0, &tv);
			// printf("tim %d %d %d\n", tim, tv.tv_sec, tv.tv_usec);
			libusb_handle_events_timeout(0, &tv);
		}
	}
}
