#ifndef __PROVIDER_TEMP_H__
#define __PROVIDER_TEMP_H__

void provTempReset();
void provTempProcess(const void* data, int len);
void provTempTmr();

void provTempSetRealSensorsCount(int cnt);
void provTempSetValueIntFrac(int num, int16_t integral, uint16_t frac);
void provTempSetValueFloat(int num, float value);
void provTempSetError(int num);

#endif
