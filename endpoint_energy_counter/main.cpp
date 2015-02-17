#include <stdio.h>
#include <unistd.h>

#include <UdpSocket.h>
#include <providers.h>

void rfInit();
void rfProcess();

void rfNewCounterValue(int val)
{
	provCounterSet(0, val);
	provCounterSendState();
}

UdpSocket socket1;

void provSendPacket(const void* buffer, int len)
{
	socket1.sendData("127.0.0.1", 9999, buffer, len);
}

class Program : public IEthernetDataListener
{
public:
	Program()
	{
		int r;
		r = socket1.init();
		socket1.setPort(10000);
		socket1.bind();
		socket1.setListener(this);
		
		rfInit();
	}
	
	void run()
	{
		provInit();
		
		for (;;)
		{
			rfProcess();
			socket1.process();
			provTmr();
			usleep(10000);
		}
	}
	
	void onEthernetDataReceived(const string& ip, const void* buffer, int len)
	{
		provProcess(buffer, len);
	}
};

void provInputResetState()
{
}
void provOutputSetOutput(int num, int enable)
{
}
void provOutputUpdate()
{
}
void provCounterResetState()
{
	provCounterSet(0, 0);
}

int main()
{
	Program p;
	p.run();
	return 0;
}
