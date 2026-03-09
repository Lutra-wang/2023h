#include "judge.h"

#define adc_clk 3600000

void analyzeWavePeaks(float rata, int wavepeak_counter,wavepeak *wavepeak_1, float *value_peak) {
    int freq_1, freq_2;

    if(1 - rata < 0.01) {
        if(wavepeak_counter == 2) {
            printf("double sin\r\n");
            freq_1 = wavepeak_1[0].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
            freq_2 = wavepeak_1[1].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
            printf("freq1=%d\r\nfreq2=%d\r\n", freq_1, freq_2);
        } else if(wavepeak_counter > 2) {
            printf("double Triangle N\r\n");
            freq_1 = wavepeak_1[0].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
            freq_2 = wavepeak_1[1].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
            printf("freq1=%d\r\nfreq2=%d\r\n", freq_1, freq_2);
        }
    } else if(1 - rata > 0.15) {
        printf("sin Triangle N\r\n");
        freq_1 = wavepeak_1[0].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
        freq_2 = wavepeak_1[1].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
        printf("sin_freq=%d\r\nTriangle_freq=%d\r\n", freq_1, freq_2);
    } else if(0.09 < 1 - rata && 1 - rata < 0.12) {
        if(value_peak[0] < value_peak[1]) {
            printf("sin Triangle Y\r\n");
            freq_1 = wavepeak_1[0].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
            freq_2 = wavepeak_1[1].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
            printf("sin_freq=%d\r\nTriangle_freq=%d\r\n", freq_1, freq_2);
        } else if(value_peak[0] > value_peak[1]) {
            printf("double Triangle Y\r\n");
            freq_1 = wavepeak_1[0].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
            freq_2 = wavepeak_1[1].wavepeak_index_s * adc_clk / ADC_SAMPLE_LEN / 2;
            printf("freq_1=%d\r\nfreq_2=%d\r\n", freq_1, freq_2);
        }
    }
}
