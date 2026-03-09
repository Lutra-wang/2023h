#ifndef __CFFT_F32_H__
#define __CFFT_F32_H__

#include "BSP.h"


//void FFT_Calcular_win(int size,float *window,float *FFT_InputBuf,float *FFT_OutputBuf,uint16_t *ADC_1_Value_DMA);
void FFT_Calcular_win(float *window,float *FFT_InputBuf,float *FFT_OutputBuf,uint16_t *ADC_1_Value_DMA);


#endif 
