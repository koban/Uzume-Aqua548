#include "t_services.h"

#include "ad1836a.h"

#include <cdefBF533.h>

#include <ccblkfn.h>



void spi_handler(void)

{

	*pDMA5_IRQ_STATUS = DMA_DONE;		// SPI DMAŠ„‚è‚İ‚ğƒNƒŠƒA

	ssync();

	isig_sem( spi_sem );

}



