#include "ees_profiler_v1_7.h"




// initialize
void EProfiler_Initialize(EProfiler *Instance, uintptr_t base_address)
{
	Instance->BASE_ADDRESS = base_address;

	// effectively deactivate counters
	for(int i=0; i<4; i++)
		EProfiler_ConfigureCounter(Instance, i, EPR_NONE , 0xFFFFFFFE,0xFFFFFFFF);

	// Set to no subsampling stretch for digital count_* output signals
	EProfiler_SetSubsampleStretchRate(Instance, 1);
}

// Counter A
void EProfiler_ConfigureCounter(EProfiler *Instance, uint8_t Channel, uint8_t Mode, uint32_t startaddr_memaddr, uint32_t endaddr_memval)
{
	volatile uint32_t *reg = (uint32_t*)Instance->BASE_ADDRESS;

    // erase old count mode
	reg[EPR_CONFIG_REG]  = 0; // EPR_NONE, no strobe, no reset
	reg[EPR_CONFIG_REG] |= (EPR_STROBE_BIT << (EPR_CTR_STROBE_SHIFT+Channel)); // set strobe
	reg[EPR_CONFIG_REG]  = 0; // clear strobe, config bits

	// reset counter
	EProfiler_ResetCounter(Instance, Channel);

    // set address range
	reg[EPR_STARTADDR_REG] = startaddr_memaddr;
	if (Mode & EPR_MEM_VAL)
		reg[EPR_ENDADDR_REG] = endaddr_memval;
	else
		reg[EPR_ENDADDR_REG] = endaddr_memval - 4;

    // set count mode, strobe
	reg[EPR_CONFIG_REG]  = (Mode << EPR_CTR_CONFIG_SHIFT); // set config bits
	reg[EPR_CONFIG_REG] |= (EPR_STROBE_BIT << (EPR_CTR_STROBE_SHIFT+Channel)); // set strobe
	reg[EPR_CONFIG_REG]  = 0; // clear strobe, config bits
}

void EProfiler_ResetCounter(EProfiler *Instance, uint8_t Channel)
{
	volatile uint32_t *reg = (uint32_t*)Instance->BASE_ADDRESS;

	reg[EPR_CONFIG_REG] |=  (EPR_RST_BIT << (EPR_CTR_RESET_SHIFT+Channel));
	reg[EPR_CONFIG_REG] &= ~(EPR_RST_BIT << (EPR_CTR_RESET_SHIFT+Channel));
}

uint32_t EProfiler_ReadCounter(EProfiler *Instance, uint8_t Channel)
{
	volatile uint32_t *reg = (uint32_t*)Instance->BASE_ADDRESS;

	return reg[EPR_READCTR_BASEREG + Channel];
}

void EProfiler_SetSubsampleStretchRate(EProfiler *Instance, uint32_t Rate)
{
	volatile uint32_t *reg = (uint32_t*)Instance->BASE_ADDRESS;

	reg[EPR_SUBSAMPLE_STRETCH_REG] = Rate;
}
