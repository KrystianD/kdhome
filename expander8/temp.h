#ifndef __TEMP_H__
#define __TEMP_H__

enum ETempStatus { TEMP_NOSENSOR, TEMP_SENSORINITIALIZED, TEMP_SENSORWAITFORREAD };

extern enum ETempStatus tempStatus;
extern int8_t tempCurrent;
extern uint8_t tempCurrentFraq;
extern uint32_t tempLastRead;

void tempInit ();
void tempProcess ();

#endif
