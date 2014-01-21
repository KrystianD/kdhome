#include "lua_funcs.h"

#include "common.h"
#include <unistd.h>

#include <lua.h>

#include "Controller.h"
#include "ICURL.h"

void sleep(int msec)
{
	usleep(msec * 1000);
}
/*
// Expander
void freezeExpander() { program->getController().freezeExpander(); }
void unfreezeExpander() { program->getController().unfreezeExpander(); }

// Sensor
int getLightValue() { return program->getController().getLightValue(); }
int getFlags() { return program->getController().getFlags(); }
void setSensitivity(int sens) { program->getController().setSensitivity(sens); }
*/

void logEvent(int category, const std::string& message)
{
	program->logEvent(category, message);
}

// Devices
int registerEthernetDevice(int id, const std::string& ip)
{
	return program->getController()->registerEthernetDevice(id, ip);
}
void addOutputProvider(int dev, int outputsCount)
{
	program->getController()->addOutputProvider(dev, outputsCount);
}
void addInputProvider(int dev, int inputsCount)
{
	program->getController()->addInputProvider(dev, inputsCount);
}
void addIRProvider(int dev)
{
	program->getController()->addIRProvider(dev);
}

// Inputs
bool getInput(int num)
{
	return program->getController()->getInput(num);
}
// Outputs
void toggleOutput(int num)
{
	program->getController()->toggleOutput(num);
}
void setOutput(int num, int on)
{
	program->getController()->setOutput(num, on);
}
bool getOutput(int num)
{
	return program->getController()->getOutput(num);
}

// Intervals
void setInterval(const std::string& id, float timeout, const std::string& code)
{
	program->getController()->setInterval(id, timeout, code);
}
void setTimeout(const std::string& id, float timeout, const std::string& code)
{
	program->getController()->setTimeout(id, timeout, code);
}
void removeTimeout(const std::string& id) { program->getController()->removeTimeout(id); }
void removeInterval(const std::string& id) { program->getController()->removeInterval(id); }
bool hasTimeout(const std::string& id) { return program->getController()->hasTimeout(id); }
bool hasInterval(const std::string& id) { return program->getController()->hasInterval(id); }

// Http
void getHttpParam(const std::string name, int* ret, std::string* value)
{
	char buf[100];
	mg_connection *conn = program->getController()->getHttpConnection();
	*ret = mg_get_var(conn, name.c_str(), buf, sizeof(buf));
	*value = buf;
}
void sendHttpReply(const std::string reply)
{
	mg_connection *conn = program->getController()->getHttpConnection();
	mg_send_data(conn, reply.c_str(), reply.size());
}

void fetchPage(const std::string url, int* ret, std::string* content)
{
	*ret = ICURL::fetchPage(url, *content);
}
