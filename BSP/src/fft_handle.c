#include "fft_handle.h"

 

uint8_t judge_tap(float base_rough)
{
  if(base_rough > ValueThreshold)
	{
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_RESET);
				return 0;//4 倍
	}
	else 
	{
		    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_SET);
				return 1;//20倍
	}

}


//高频不是20倍，前20个点不能用
void adc1_tim6_getdata_4M(uint32_t *ADC_1_Value_DMA,uint16_t size)
{
	HAL_TIM_Base_Stop(my_htim);
	TIM6->PSC = 12-1;
	HAL_ADC_Start_DMA(my_hadc, (uint32_t *)ADC_1_Value_DMA, size);
	HAL_TIM_Base_Start(my_htim);
}

void adc1_tim6_getdata_fit(uint32_t *ADC_1_Value_DMA,uint16_t size,uint16_t TIM6_psc)
{
	HAL_TIM_Base_Stop(my_htim);
	TIM6->PSC = TIM6_psc-1;
	HAL_ADC_Start_DMA(my_hadc, (uint32_t *)ADC_1_Value_DMA, size);
	HAL_TIM_Base_Start(my_htim);
}



void baseFreq_index(float *FFT_OutputBuf, uint16_t size,float *basewave_value,uint16_t *basewave_index)
{

		arm_max_f32(FFT_OutputBuf,size,basewave_value,(uint32_t *)basewave_index);
}

//波峰索引
void waveindex_get(float *FFT_OutputBuf, uint16_t *waveindex,uint8_t *wavepeak_counter,float *basewave_value) 
{
	  for(int i= 0;i<wave_counter_max;i++)
				waveindex[i]=0;
		float doorvalue = *basewave_value * doorvalue_ratio;
		int j=0;
		for(int i=2;i<ADC_SAMPLE_LEN/2-2;i++)
	  {
			 
	     if(FFT_OutputBuf[i]>doorvalue)
				 if(FFT_OutputBuf[i]>FFT_OutputBuf[i-1]&&FFT_OutputBuf[i]>FFT_OutputBuf[i-2])
					 if(FFT_OutputBuf[i]>FFT_OutputBuf[i+1]&&FFT_OutputBuf[i]>FFT_OutputBuf[i+2])
					 {
							waveindex[j] = i;
						  j++;
						 *wavepeak_counter += 1;
					 }
		if(j == wave_counter_max)
			break;
		}
	
}

void value_peak_corr(float *value_peak,uint8_t value_counter,float *FFT_OutputBuf,uint16_t *waveindex)
{
	for(int i = 0;i<value_counter;i++)
	{  
		value_peak[i] = sqrt((pow(FFT_OutputBuf[waveindex[i]-3], 2)+pow(FFT_OutputBuf[waveindex[i]-2], 2)
		                +pow(FFT_OutputBuf[waveindex[i]-1], 2)+pow(FFT_OutputBuf[waveindex[i]], 2)+
										pow(FFT_OutputBuf[waveindex[i]+3], 2)+pow(FFT_OutputBuf[waveindex[i]+2], 2)
		                +pow(FFT_OutputBuf[waveindex[i]+1], 2))*8/3);
	}

}
