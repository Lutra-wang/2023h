/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "memorymap.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP.h"

#include "judge.h"

#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t rx_data;

volatile uint8_t adc_flag = 0,adc_cpltflag = 0;

float Window[ADC_SAMPLE_LEN] = {0};//

volatile uint16_t tim6_psc = 0;    //


arm_cfft_radix4_instance_f32 scfft;//定义scfft结构体
float FFT_InputBuf[ADC_SAMPLE_LEN*2];	//FFT输入数组�
float FFT_OutputBuf[ADC_SAMPLE_LEN];	//FFT输出数组

static uint16_t ADC_Value_DMA[ADC_SAMPLE_LEN+ADC_FAULTSAMPLE_LEN] = {0};//包含坏值的adc采样
uint16_t* valid_data_start = NULL;          // 有效数据起始指针

float basewave_value = 0;					//基波幅值
uint16_t basewave_index = 0;			//基波索引

uint16_t wavepeak_index[wave_counter_max] = {0};//波峰索引数组
uint8_t wavepeak_counter = 0;// 波峰索引个数

float value_peak[wave_counter_max] = {0};//波峰（拟合后）
//float value_peak_sort[wave_counter_max] = {0};//波峰（排序后）

uint32_t freq_1 = 0,freq_2 = 0;



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//结构体排序
int compare_wavepeak(const void *a, const void *b) {
    wavepeak *peakA = (wavepeak *)a;
    wavepeak *peakB = (wavepeak *)b;
    
    if (peakA->value_peak_s < peakB->value_peak_s) return 1;   // 降序排序
    if (peakA->value_peak_s > peakB->value_peak_s) return -1;
    return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM6_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	Hanning_Win(ADC_SAMPLE_LEN ,Window);
	HAL_UART_Receive_IT(&huart1,&rx_data,1);
	HAL_ADCEx_Calibration_Start(my_hadc,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED);
	HAL_Delay(10);
	printf("init OK\r\n");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(adc_flag)
		{
			adc_flag = 0;
			//采集ADC_SAMPLE_LEN+ADC_FAULTSAMPLE_LEN个数据
			adc1_tim6_getdata_4M((uint32_t *)ADC_Value_DMA,ADC_SAMPLE_LEN+ADC_FAULTSAMPLE_LEN);
			
			//等待采样完成
			while(!adc_cpltflag){};
			
			//用指针去除前ADC_FAULTSAMPLE_LEN个坏值
			valid_data_start = &ADC_Value_DMA[ADC_FAULTSAMPLE_LEN];
			adc_cpltflag = 0;
			//for(int i=ADC_FAULTSAMPLE_LEN;i<ADC_SAMPLE_LEN+ADC_FAULTSAMPLE_LEN;i++) printf("adc1=%d,",ADC_Value_DMA[i]);//√correct
				
			//4096个点的fft计算
			FFT_Calcular_win(Window,FFT_InputBuf,FFT_OutputBuf,valid_data_start);	
			//for(int i=0;i<ADC_SAMPLE_LEN/2;i++) printf("adc1=%f,",FFT_OutputBuf[i]);
				
			//hanning窗函数检验
		  //for(int i=0;i<ADC_SAMPLE_LEN;i++)printf("adc1=%f,",Window[i]);√correct
				
			//获取基波幅值和索引
		  baseFreq_index(FFT_OutputBuf,ADC_SAMPLE_LEN,&basewave_value,&basewave_index);
			//printf("\r\nbasewave_value=%f,basewave_index=%d",basewave_value,basewave_index);//√correct
				
			//波峰索引和个数获取
			waveindex_get(FFT_OutputBuf,wavepeak_index,&wavepeak_counter,&basewave_value);
			//for(int i = 0;i<wavepeak_counter;i++) printf("\r\n%d",wavepeak_index[i]);//√correct
			
			//能量重心矫正峰值
			value_peak_corr(value_peak,wavepeak_counter,FFT_OutputBuf,wavepeak_index);
			
			//定义长度可变的结构体并赋值（绑定索引和相应幅值）
			wavepeak wavepeak_1[wavepeak_counter];
			for(int i = 0;i<wavepeak_counter;i++)
				{
					wavepeak_1[i].wavepeak_index_s = wavepeak_index [i];
					wavepeak_1[i].value_peak_s = value_peak[i];
					//printf("wavepeak_index_s=%d\r\n value_peak_s=%f\r\n",wavepeak_1[i].wavepeak_index_s,wavepeak_1[i].value_peak_s);//correct
				}
				//结构体排序
				qsort(wavepeak_1, wavepeak_counter, sizeof(wavepeak), compare_wavepeak);
//				for(int i = 0;i<wavepeak_counter;i++)
				//验证排序
//					printf("wavepeak_index_s=%d\r\n value_peak_s=%f\r\n",wavepeak_1[i].wavepeak_index_s,wavepeak_1[i].value_peak_s);//correct

				//计算两峰值比
				float  rata = wavepeak_1[1].value_peak_s / wavepeak_1[0].value_peak_s;
					printf("rata=%f\r\n",rata);
/****************************************************************模板分界线************************************************************************/					
				
				analyzeWavePeaks(rata,wavepeak_counter,wavepeak_1,value_peak);
				//波形识别&计算频率（频率高于100k识别不准）			
		}
		
		
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	//HAL_ADC_Stop_DMA(&hadc1);
	HAL_TIM_Base_Stop(&htim6);
	adc_cpltflag ++;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		if(rx_data == '1')
			adc_flag = 1;
	}

	HAL_UART_Receive_IT(&huart1,&rx_data,1);
}


/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
