#include "ees_twinspi_v1_0.h"

// initialize
void ETwinSPI_Initialize(ETwinSPI *Instance, uintptr_t base_address, uint16_t clock_div, uint8_t CPHA, uint8_t CPOL)
{
	Instance->BASE_ADDRESS = base_address;
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);
	Regs[ETS_CONFIG_REG] = ((clock_div << ETS_ixDIV) & ETS_bmDIV) | ((CPHA << ETS_ixCPHA) & ETS_bmCPHA) | ((CPOL << ETS_ixCPOL) & ETS_bmCPOL);
}


// SPI operations
void ETwinSPI_WriteOnly(ETwinSPI *Instance, uint8_t Channels, uint8_t size, uint8_t *buf_A, uint8_t *buf_B)
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);

	// configure write pattern
	uint32_t wr_mask = 0;
	for(int i=0; i<size; i++) {
		wr_mask <<= 1;
		wr_mask |= 1;
	}
	Regs[ETS_PATTERN_REG] = ( (wr_mask << ETS_ixWR_PATTERN) & ETS_bmWR_PATTERN );

	// Write data into send FIFOs
	if (buf_A != NULL)
		for(int i=0; i<size; i++)
			Regs[ETS_TX_A_FIFO_REG] = *buf_A++;

	if (buf_B != NULL)
		for(int i=0; i<size; i++)
			Regs[ETS_TX_B_FIFO_REG] = *buf_B++;

	Regs[ETS_CTRL_REG] &= ~ETS_bmNSYNC; // clear old channel selection
	Regs[ETS_CTRL_REG] |= ((Channels << ETS_ixNSYNC) & ETS_bmNSYNC); // set channel(s)
	Regs[ETS_CTRL_REG] |= ((1 << ETS_ixST) & ETS_bmST); // Start
	Regs[ETS_CTRL_REG] &= ~((1 << ETS_ixST) & ETS_bmST);
}

// 
void ETwinSPI_ReadBlocking(ETwinSPI *Instance, uint8_t size, uint8_t *buf_A, uint8_t *buf_B)
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);

	// Wait for completed SPI transaction
	while ((Regs[ETS_STATUS_REG] & ETS_bmDONE) == 0);

	// Read data from receive FIFOs
	if (buf_A != NULL)
		for(int i=0; i<size; i++)
		*buf_A++ = Regs[ETS_RX_A_FIFO_REG];

	if (buf_B != NULL)
		for(int i=0; i<size; i++)
		*buf_B++ = Regs[ETS_RX_B_FIFO_REG];

}

int  ETwinSPI_ReadNonBlocking(ETwinSPI *Instance, uint8_t size, uint8_t *buf_A, uint8_t *buf_B)
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);
	
	if ((Regs[ETS_STATUS_REG] & ETS_bmDONE) == 0)
		return 0;

	// Read data from receive FIFOs
	if (buf_A != NULL)
		for(int i=0; i<size; i++)
		*buf_A++ = Regs[ETS_RX_A_FIFO_REG];

	if (buf_B != NULL)
		for(int i=0; i<size; i++)
		*buf_B++ = Regs[ETS_RX_B_FIFO_REG];

	return 1;
}

void ETwinSPI_WriteAndReadBlocking(ETwinSPI *Instance, uint8_t Channels,
                                    uint8_t wrsize, uint8_t *wrbuf_A, uint8_t *wrbuf_B,
                                    uint8_t rdsize, uint8_t *rdbuf_A, uint8_t *rdbuf_B )
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);

	// configure write and read pattern
	// writes and reads sequential, not overlapping - need custom function for that (TODO)
	uint32_t wr_mask = 0;
	uint32_t rd_mask = 0;
	for(int i=0; i<wrsize; i++) {
		wr_mask <<= 1;
		wr_mask |= 1;
		rd_mask <<= 1; // rd_mask |= 0;
	}
	for(int i=0; i<rdsize; i++) {
		rd_mask <<= 1;
		rd_mask |= 1;
	}
	uint32_t pattern = ( (rd_mask << ETS_ixRD_PATTERN) & ETS_bmRD_PATTERN )
							| ( (wr_mask << ETS_ixWR_PATTERN) & ETS_bmWR_PATTERN );
	Regs[ETS_PATTERN_REG] = pattern;

	// Write data into send FIFOs
	if (wrbuf_A != NULL)
		for(int i=0; i<wrsize; i++)
			Regs[ETS_TX_A_FIFO_REG] = *wrbuf_A++;

	if (wrbuf_B != NULL)
		for(int i=0; i<wrsize; i++)
			Regs[ETS_TX_B_FIFO_REG] = *wrbuf_B++;

	Regs[ETS_CTRL_REG] &= ~ETS_bmNSYNC; // clear old channel selection
	Regs[ETS_CTRL_REG] |= ((Channels << ETS_ixNSYNC) & ETS_bmNSYNC); // set channel(s)
	Regs[ETS_CTRL_REG] |= ((1 << ETS_ixST) & ETS_bmST); // Start
	Regs[ETS_CTRL_REG] &= ~((1 << ETS_ixST) & ETS_bmST);

	// Wait for completed SPI transaction
	while ((Regs[ETS_STATUS_REG] & ETS_bmDONE) == 0);

	// Read data from receive FIFOs
	if (rdbuf_A != NULL)
		for(int i=0; i<rdsize; i++)
		*rdbuf_A++ = Regs[ETS_RX_A_FIFO_REG];

	if (rdbuf_B != NULL)
		for(int i=0; i<rdsize; i++)
		*rdbuf_B++ = Regs[ETS_RX_B_FIFO_REG];

}

void ETwinSPI_WaitBlocking(ETwinSPI *Instance)
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);

	while ((Regs[ETS_STATUS_REG] & ETS_bmDONE) == 0);
}

int  ETwinSPI_WaitNonBlocking(ETwinSPI *Instance)
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);

	if (Regs[ETS_STATUS_REG] & ETS_bmDONE)
		return 1;
	else
		return 0;
}


// Interrupt management
void ETwinSPI_EnableInterrupt(ETwinSPI *Instance)
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);
	Regs[ETS_CTRL_REG] |= ETS_bmIE;
}

void ETwinSPI_DisableInterrupt(ETwinSPI *Instance)
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);
	Regs[ETS_CTRL_REG] &= ~ETS_bmIE;
}

void ETwinSPI_ClearInterrupt(ETwinSPI *Instance)
{
	volatile uint32_t *Regs = (uint32_t*)(Instance->BASE_ADDRESS);
	Regs[ETS_CTRL_REG] &= ~ETS_bmIE;
	Regs[ETS_CTRL_REG] |= ETS_bmIE;
}
