#ifndef EES_TWINSPI_V1_0_H
#define EES_TWINSPI_V1_0_H

#include <stddef.h>
#include <stdint.h>


// IP REGISTER INDICES
#define ETS_CTRL_REG   0
#define ETS_CONFIG_REG 1
#define ETS_PATTERN_REG 2
#define ETS_STATUS_REG 3

#define ETS_TX_A_FIFO_REG 4
#define ETS_TX_B_FIFO_REG 5
#define ETS_RX_A_FIFO_REG 6
#define ETS_RX_B_FIFO_REG 7


// COUNTER AND STATUS BITS - BITMASKS AND SHIFT INDEXES
#define ETS_bmST  0x00000001
#define ETS_bmIE  0x00000002
#define ETS_bmNSYNC 0x000F0000 // bit enables for {nSS3,nSS2,nSS1,nSS0} channels
#define ETS_CH0 0x0001 // nSS0 channel
#define ETS_CH1 0x0002 // nSS1 channel
#define ETS_CH2 0x0004 // nSS2 channel
#define ETS_CH3 0x0008 // nSS3 channel
#define ETS_ixST  0
#define ETS_ixIE  1
#define ETS_ixNSYNC 16

#define ETS_bmDIV  0x0000FFFF
#define ETS_bmCPHA 0x00010000
#define ETS_bmCPOL 0x00020000
#define ETS_ixDIV  0
#define ETS_ixCPHA 16
#define ETS_ixCPOL 17

#define ETS_bmWR_PATTERN 0x0000FFFF
#define ETS_ixWR_PATTERN 0
#define ETS_bmRD_PATTERN 0xFFFF0000
#define ETS_ixRD_PATTERN 16

#define ETS_bmDONE 0x00000001
#define ETS_ixDONE 0x00000001


typedef struct {
    uintptr_t BASE_ADDRESS;
} ETwinSPI;


// FUNCTION PROTOTYPES
void ETwinSPI_Initialize(ETwinSPI *Instance, uintptr_t base_address, uint16_t clock_div, uint8_t CPHA, uint8_t CPOL);

void ETwinSPI_WriteOnly(ETwinSPI *Instance, uint8_t Channels, uint8_t size, uint8_t *buf_A, uint8_t *buf_B); // No blocking required
void ETwinSPI_ReadBlocking(ETwinSPI *Instance, uint8_t size, uint8_t *buf_A, uint8_t *buf_B);
int  ETwinSPI_ReadNonBlocking(ETwinSPI *Instance, uint8_t size, uint8_t *buf_A, uint8_t *buf_B);
void ETwinSPI_WriteAndReadBlocking(ETwinSPI *Instance, uint8_t Channels,
                                    uint8_t wrsize, uint8_t *wrbuf_A, uint8_t *wrbuf_B,
                                    uint8_t rdsize, uint8_t *rdbuf_A, uint8_t *rdbuf_B );
void ETwinSPI_WaitBlocking(ETwinSPI *Instance);
int  ETwinSPI_WaitNonBlocking(ETwinSPI *Instance);

void ETwinSPI_EnableInterrupt(ETwinSPI *Instance);
void ETwinSPI_DisableInterrupt(ETwinSPI *Instance);
void ETwinSPI_ClearInterrupt(ETwinSPI *Instance);

// TODO: Custom Write/Read overlaps

#endif // EES_TWINSPI_V1_0_H
