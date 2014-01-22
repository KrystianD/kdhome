#include <stdint.h>

#include <color_codes.h>

typedef uint32_t u32_t;
typedef int32_t s32_t;
typedef uint16_t u16_t;
typedef int16_t s16_t;
typedef uint8_t u8_t;
typedef int8_t s8_t;

typedef u32_t               mem_ptr_t;

#define X16_F "x"
#define X32_F "x"
#define S16_F "d"
#define S32_F "d"
#define U32_F "u"
#define U16_F "u"

#define LWIP_PLATFORM_DIAG2(x,...) myprintf (x "\r", ##__VA_ARGS__)
#define LWIP_PLATFORM_DIAG(x) LWIP_PLATFORM_DIAG2 x

#define LWIP_PLATFORM_ASSERT(x) myprintf (COLOR_RED "[assert] " x COLOR_OFF "\r\n")

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END

// #define LWIP_PLATFORM_ASSERT(expr)

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#define LWIP_PLATFORM_BYTESWAP 0
typedef unsigned char    sys_prot_t;
