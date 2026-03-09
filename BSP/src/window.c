#include "window.h"

void Hanning_Win (int N,float *window)	
{
    for (int n = 0; n < N; ++n)
  	{
        window[n] = 0.5 * (1 - cos(2 * M_PI * n / (N - 1)));
    }
}

