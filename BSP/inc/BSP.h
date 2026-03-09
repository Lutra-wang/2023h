#include "main.h"
#include "adc.h"
#include "dma.h"
#include "memorymap.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#include "window.h"
#include "fft_handle.h"
#include "cfft_f32.h"

#define my_hadc &hadc1
#define my_htim &htim6
#define ADC_SAMPLE_LEN          4096  //ADC??????????   
#define ADC_FAULTSAMPLE_LEN     20  //ADC??????????  
#define wave_counter_max        10      //»ù²¨+Ð³²¨´ÎÊý
#define doorvalue_ratio         0.01

extern volatile uint8_t adc_flag,adc_cpltflag;

extern float Window[ADC_SAMPLE_LEN];//

extern volatile uint16_t tim6_psc;    //
