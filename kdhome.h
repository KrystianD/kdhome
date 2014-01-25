#ifndef __HS_H__
#define __HS_H__

#define PROVIDER_TYPE_CONTROL 0x0000
#define PROVIDER_TYPE_OUTPUT  0x0001
#define PROVIDER_TYPE_INPUT   0x0002
#define PROVIDER_TYPE_IR      0x0003
#define PROVIDER_TYPE_TEMP    0x0004

// Control
#define CONTROL_CMD_REGISTER    0x01

// Output provider
#define OUTPUT_CMD_SET_OUTPUTS  0x00

// Input provider
#define INPUT_NOTF_NEWSTATE     0x00
#define INPUT_REQ_SENDSTATE     0x01

// IR provider
#define IR_NOTF_NEWCODE         0x00

// Temperature provider
#define TEMP_NOTF_TEMP          0x00

#endif
