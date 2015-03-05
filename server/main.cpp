#include <getopt.h>

#include "Program.h"
#include "Controller.h"

#include <kdutils.h>

#include "common.h"
#include "MyLogger.h"

using namespace std;

MyLogger *logger, *userLogger;
Program *program;
Controller controller;
// libusb_context *ctx;

int main(int argc, char** argv)
{
	string logFile = "kdhome.log";
	string userLogFile = "kdhome_user.log";
	while (1)
	{
		static struct option long_options[] =
		{
			{ "log",  required_argument, 0, 'l' },
			{ "user-log",  required_argument, 0, 'u' },
			{ 0, 0, 0, 0 }
		};
		int option_index = 0;
		
		int c = getopt_long(argc, argv, "l:", long_options, &option_index);
		                
		if (c == -1)
			break;
			
		switch (c)
		{
		// case 0:
			// /* If this option set a flag, do nothing else now. */
			// if (long_options[option_index].flag != 0)
				// break;
			// printf("option %s", long_options[option_index].name);
			// if (optarg)
				// printf(" with arg %s", optarg);
			// printf("\n");
			// break;
		case 'l':
			logFile = optarg;
			printf("option -l with value `%s'\n", optarg);
			break;
		case 'u':
			userLogFile = optarg;
			printf("option -u with value `%s'\n", optarg);
			break;
		default:
			abort();
		}
	}
	// libusb_init (&ctx);
	
	MyLogger logger;
	ConsoleLoggerOutput out1;
	out1.enableColors();
	FileLoggerOutput out2(logFile);
	logger.addOutput(&out1);
	logger.addOutput(&out2);
	
	::logger = &logger;

	MyLogger userLogger;
	FileLoggerOutput out3(userLogFile);
	userLogger.addOutput(&out1);
	userLogger.addOutput(&out3);
	
	::logger = &logger;
	::userLogger = &userLogger;

	srand(time(0));

	controller.setLogger(&logger);
	controller.setUserLogger(&userLogger);
	if(!controller.init())
	{
		logger.logError("Unable to init controller");
		return 1;
	}

	controller.run();
	
	// libusb_exit (ctx);
}
