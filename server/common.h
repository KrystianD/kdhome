#ifndef __COMMON_H__
#define __COMMON_H__

#include "format.h"

#include "MyLogger.h"
#include "Program.h"
#include "settings.h"
using namespace fmt;

struct libusb_context;

extern Program *program;
extern MyLogger *logger;
extern libusb_context *ctx;

#endif
