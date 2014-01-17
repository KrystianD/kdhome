#include "Program.h"

#include <kutils.h>

#include "common.h"
#include "MyLogger.h"

using namespace std;

MyLogger *logger;
Program *program;
// libusb_context *ctx;

int main(int argc, char** argv)
{
	// libusb_init (&ctx);

	MyLogger logger;
	ConsoleLoggerOutput out1;
	out1.enableColors();
	FileLoggerOutput out2("kdhome.log");
	logger.addOutput(&out1);
	logger.addOutput(&out2);

	::logger = &logger;
	
	Program p;
	::program = &p;
	p.setLogger(&logger);
	p.run();

	// libusb_exit (ctx);
}
