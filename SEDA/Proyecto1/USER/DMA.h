#if !defined (_DMA_H_)
#define _DMA_H_

void play(void);
void rec(void);
void init_DMA_DAC(void);
void init_ADC_grabar(void);
void init_TIMER1(void);
void init_DMA_ADC(void);
void DMA_IRQHandler(void);
void EINT2_IRQHandler();
void init_DMA(void);
extern int q,w,e,r,t;

#endif //_DMA_H_