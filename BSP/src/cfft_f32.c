#include "cfft_f32.h"
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"



void FFT_Calcular_win(float *window,float *FFT_InputBuf,float *FFT_OutputBuf,uint16_t *ADC_1_Value_DMA)
{

	float average = 0;
	float adc_value;
	
	for(uint16_t i=0; i < ADC_SAMPLE_LEN; i++) 
	{
		average += ADC_1_Value_DMA[i]*3.3/65536;
		
	}
	average /= ADC_SAMPLE_LEN;
//输入加窗√
	for(uint16_t i=0; i < ADC_SAMPLE_LEN; )
{
    adc_value = ADC_1_Value_DMA[i] * 3.3f / 65536.0f;
    FFT_InputBuf[2*i] = (adc_value - average) * window[i]; // 实部
    FFT_InputBuf[2*i+1] = 0.0f;                             // 虚部
		i=i+1;
}
    arm_cfft_f32(&arm_cfft_sR_f32_len4096, FFT_InputBuf, 0, 1);
		arm_cmplx_mag_f32(FFT_InputBuf,FFT_OutputBuf,ADC_SAMPLE_LEN);	//取模得幅值
		for(int i = 0;i<ADC_SAMPLE_LEN;i++)
		FFT_OutputBuf[i] /=(ADC_SAMPLE_LEN/2);
}



