#ifndef __HS_H__
#define __HS_H__

#pragma pack(1)

typedef struct
{
	uint16_t packetId, sessKey, type;
	uint8_t cmd;
} TProvHeader;

typedef struct
{
	uint16_t packetId, type;
	uint8_t cmd;
} TSrvHeader;

#define PROVIDER_TYPE_CONTROL 0x0000
#define PROVIDER_TYPE_OUTPUT  0x0001
#define PROVIDER_TYPE_INPUT   0x0002
#define PROVIDER_TYPE_IR      0x0003
#define PROVIDER_TYPE_TEMP    0x0004

// Control
#define CONTROL_CMD_REGISTER    0x00
typedef struct
{
	TSrvHeader header;
	uint16_t sessKey;
} TSrvCmdRegister;

// Output provider
#define OUTPUT_CMD_REGISTER     0x00
typedef struct
{
	TProvHeader header;
	uint8_t cnt;
} TProvOutputRegisterPacket;
#define OUTPUT_CMD_SET_OUTPUTS  0x01
typedef struct
{
	TSrvHeader header;
	uint8_t cnt;
	uint32_t outputs;
} TSrvOutputSetOutputsPacket;

// Input provider
#define INPUT_CMD_REGISTER      0x00
typedef struct
{
	TProvHeader header;
	uint8_t cnt;
} TProvInputRegisterPacket;
#define INPUT_NOTF_NEWSTATE     0x01
#define INPUT_REQ_SENDSTATE     0x02
typedef struct
{
	TProvHeader header;
	uint8_t cnt;
	struct
	{
		uint8_t high, low;
	} inputs[8];
} TProvInputStatePacket;

// IR provider
#define IR_CMD_REGISTER         0x00
typedef struct
{
	TProvHeader header;
} TProvIRRegisterPacket;
#define IR_NOTF_NEWCODE         0x01
typedef struct
{
	TProvHeader header;
	uint32_t code;
} TProvIRCodePacket;

// Temperature provider
#define TEMP_CMD_REGISTER       0x00
typedef struct
{
	TProvHeader header;
	uint8_t cnt;
} TProvTempRegisterPacket;
#define TEMP_NOTF_TEMP          0x01
typedef struct
{
	TProvHeader header;
	uint8_t cnt, num, flags;
	union
	{
		struct
		{
			int16_t integral;
			uint16_t frac;
		} spl;
		float value;
	} value;
} TProvTempValuePacket;

#pragma pack()

#endif
