void DMA_IRQHandler(void)
{
	if (counter <= 4)
	{
		index = TAM_BLOCK_DMA * counter;	//Aumentamos índice búffer muestras
		
		if (LPC_GPDMA->DMACIntStat & 2) 		//Interrupción CH1? 
		{			
			LPC_GPDMA->DMACIntTCClear = 2;		// borramos int.Ch1
			init_DMA_ADC();
		}
		else									// Como solo tenemos dos canales en uso, este sería el canal 0
		{
			LPC_GPDMA->DMACIntTCClear = 1;		// borramos int.Ch0
			init_DMA_DAC();
		}
		counter++;								
	else
	{
		NVIC_DisableIRQ(DMA_IRQn);
		counter = 1;							// Comienza en uno, porque la primera vez que se llama a init_DMA_DAC() o init_DMA_ADC()
	}											// es desde las EINT0-1, con lo cual ya tendríamos las 4000 primeras muestras guardadas
}												// en la primera interrupción del DMA y tendríamos que empezar por muestras[4000] 	