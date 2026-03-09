# 2023h - 信号分离装置

基于 **STM32H7** 的信号分离装置项目，用于完成 **2023 年全国大学生电子设计竞赛 H 题（本科组）**。  
系统对输入混合信号进行高速采样，结合窗函数与 FFT 频谱分析实现波形识别，并完成信号分离与频率分析。题目要求加法器输出 `C=A+B`，再由分离电路恢复 `A'` 与 `B'`。:contentReference[oaicite:2]{index=2}

---

## 项目简介

本项目面向竞赛题“信号分离装置”设计，主要完成以下任务：

- 对输入混合信号进行 ADC 采样
- 对采样数据去除无效点
- 进行窗函数处理与 FFT 频谱分析
- 提取基波及多个峰值特征
- 识别输入波形类型并计算频率
- 通过串口指令触发一次完整分析流程

根据题目要求，系统需要对两路周期信号进行分离，并保证输出波形无明显失真，能够稳定显示。:contentReference[oaicite:3]{index=3} :contentReference[oaicite:4]{index=4}

---

## 功能特点

- 基于 STM32H743 平台
- 使用 ADC + DMA 实现高速采样
- 使用 TIM6 控制采样节奏
- 使用 Hanning 窗降低频谱泄漏
- 基于 CMSIS-DSP 完成 FFT 运算
- 支持波峰提取、幅值修正、排序与比值分析
- 支持串口触发采样与识别

---

## 项目结构

```text
2023h
├─ Application
│  ├─ MDK-ARM
│  │  └─ startup_stm32h743xx.s
│  └─ User/Core
│     ├─ main.c
│     ├─ gpio.c
│     ├─ adc.c
│     ├─ dma.c
│     ├─ memorymap.c
│     ├─ tim.c
│     ├─ usart.c
│     ├─ stm32h7xx_it.c
│     └─ stm32h7xx_hal_msp.c
├─ Drivers
│  ├─ STM32H7xx_HAL_Driver
│  └─ CMSIS
├─ BSP
│  ├─ cfft_f32.c
│  ├─ fft_handle.c
│  ├─ window.c
│  ├─ window.h
│  ├─ math.h
│  ├─ BSP.h
│  └─ arm_cortexM7lfsp_math.lib
├─ user
│  └─ judge.c
└─ CMSIS
