#ifndef __FFT_HANDLE_H__
#define __FFT_HANDLE_H__

#include "BSP.h"


#define FreqThreshold       10000  
#define ValueThreshold      0.12



uint8_t judge_tap(float base_rough);
void adc1_tim6_getdata_4M(uint32_t *ADC_1_Value_DMA,uint16_t size);

void adc1_tim6_getdata_fit(uint32_t *ADC_1_Value_DMA,uint16_t size,uint16_t tim8_psc);

void baseFreq_index(float *FFT_OutputBuf, uint16_t size,float *basewave_value,uint16_t *basewave_index);

void waveindex_get(float *FFT_OutputBuf,uint16_t *waveindex,uint8_t *wavepeak_counter,float *basewave_value);

void value_peak_corr(float *value_peak,uint8_t value_counter,float *FFT_OutputBuf,uint16_t *waveindex);

#endif 

