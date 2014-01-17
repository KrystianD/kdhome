#include <string>

#include <kutils.h>
/*
// Expander
void freezeExpander();
void unfreezeExpander();

// Sensor
int getLightValue();
int getFlags();
void setSensitivity(int sens);
*/

// Devices
int registerEthernetDevice(int id, const std::string& ip);
void addOutputProvider(int dev, int outputsCount);
void addInputProvider(int dev, int inputsCount);
void addIRProvider(int dev);

// Inputs
bool getInput(int num);

// Outputs
void toggleOutput(int num);
void setOutput(int num, int on);
bool getOutput(int num);

// Intervals
void setStateCheckTimeout(float timeout);
void setTimeout(const std::string& id, float timeout, const std::string& code);
void setInterval(const std::string& id, float timeout, const std::string& code);
void removeTimeout(const std::string& id);
void removeInterval(const std::string& id);
bool hasTimeout(const std::string& id);
bool hasInterval(const std::string& id);

// Http
void getHttpParam(unsigned long long conn, const std::string name, int* ret=0, std::string* value=0);
void sendHttpReply(unsigned long long conn, const std::string reply);

void fetchPage(const std::string url, int* ret = 0, std::string* content = 0);

void logEvent(int category, const std::string& message);
