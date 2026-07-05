#ifndef EES_PROFILER_V1_7_H
#define EES_PROFILER_V1_7_H

#include <stddef.h>
#include <stdint.h>


// counter IDs
#define EPR_NUM_COUNTERS 6

#define EPR_CH_A 0
#define EPR_CH_B 1
#define EPR_CH_C 2
#define EPR_CH_D 3
#define EPR_CH_E 4
#define EPR_CH_F 5


// IP REGISTER INDICES
#define EPR_CONFIG_REG       0

#define EPR_STARTADDR_REG 1
#define EPR_ENDADDR_REG   2

#define EPR_READCTR_BASEREG 3

#define EPR_SUBSAMPLE_STRETCH_REG  13

#define EPR_IRQ_CONFIG_REG      14
#define EPR_IRQ_THRESHOLD_REG   15



// COUNTER CONFIGURATION  (for EPR_CONFIG_REG)
#define EPR_RST_BIT 0x1
#define EPR_STROBE_BIT 0x1

#define EPR_READS 0x1
#define EPR_WRITES 0x2
#define EPR_EXEC_NO_BRANCHES 0x4
#define EPR_EXEC_WITH_BRANCHES 0x8
#define EPR_EXEC_NO_SUBCALLS 0x4
#define EPR_EXEC_WITH_SUBCALLS 0x8
#define EPR_MEM_VAL 0x10
#define EPR_NONE 0x0

#define EPR_CTR_RESET_SHIFT 0
#define EPR_CTR_STROBE_SHIFT 10
#define EPR_CTR_CONFIG_SHIFT 20



typedef struct {
    uintptr_t BASE_ADDRESS;
} EProfiler;

#define U32(X) ((uint32_t)(X))

// FUNCTION PROTOTYPES
void EProfiler_Initialize(EProfiler *Instance, uintptr_t base_address);
void EProfiler_ConfigureCounter(EProfiler *Instance, uint8_t Channel, uint8_t Mode, uint32_t startaddr_memaddr, uint32_t endaddr_memval);
void EProfiler_ResetCounter(EProfiler *Instance, uint8_t Channel);
uint32_t EProfiler_ReadCounter(EProfiler *Instance, uint8_t Channel);
void EProfiler_SetSubsampleStretchRate(EProfiler *Instance, uint32_t Rate);

#endif // EES_PROFILER_V1_7_H
