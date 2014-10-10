#include "UdpServer.h"

#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "kdutils.h"

UdpServer::UdpServer() : m_listener(0), m_sockfd(0)
{
}
UdpServer::~UdpServer()
{
}

bool UdpServer::init()
{
	if ((m_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		m_lastErrorStr = string("cannot create socket: ") + getErrnoString();
		return false;
	}

	struct sockaddr_in myaddr;

	memset((char*)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(9999);

	if (bind(m_sockfd, (struct sockaddr*)&myaddr, sizeof(myaddr)) < 0)
	{
		m_lastErrorStr = string("bind failed: ") + getErrnoString();
		return false;
	}

	return true;
}
void UdpServer::deinit()
{
	close(m_sockfd);
}
bool UdpServer::process()
{
	timeval tv;
	fd_set fds;

	tv.tv_sec = 0;
	tv.tv_usec = 10000;

	FD_ZERO(&fds);
	FD_SET(m_sockfd, &fds);

	// checking for new incoming connections
	int res = select(m_sockfd + 1, &fds, 0, 0, &tv);
	if (res == -1)
	{
		if (errno != EINTR)
		{
			m_lastErrorStr = string("select failed: ") + getErrnoString();
			return false;
		}
	}
	else
	{
		if (FD_ISSET(m_sockfd, &fds))
		{
			struct sockaddr_in remaddr;
			socklen_t addrlen = sizeof(remaddr);
			char buf[1500];
			int recvlen = recvfrom(m_sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&remaddr, &addrlen);

			char ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(remaddr.sin_addr), ip, sizeof(ip));

			ByteBuffer buffer(buf, recvlen);
			if (m_listener)
				m_listener->onEthernetDataReceived(ip, buffer);
		}
	}

	return true;
}

void UdpServer::sendData(const string& ip, ByteBuffer& buffer)
{
	sendData(ip, buffer.ptr(), buffer.size());
}
void UdpServer::sendData(const string& ip, const void* data, int len)
{
	struct sockaddr_in remaddr;
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(9999);
	inet_pton(AF_INET, ip.c_str(), &(remaddr.sin_addr));

	// buffer.print();
	if (sendto(m_sockfd, data, len, 0, (struct sockaddr*)&remaddr, sizeof(remaddr)) < 0)
	{
		printf("send fail\n");
	}
	else
	{
		// printf("send OK\n");
	}
}
