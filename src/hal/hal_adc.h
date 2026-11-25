void HAL_ADC_Init(int pinNumber);
int HAL_ADC_Read(int pinNumber);
// Raw reading of the chip's internal supply (VBAT) channel, where the
// platform has one. Returns a negative value when it is not supported.
int HAL_ADC_ReadVBAT(void);
void HAL_ADC_Deinit(int pinNumber);
#if defined(PLATFORM_W800)  || defined(PLATFORM_W600)
float HAL_ADC_Temp(void);
#endif
