#ifndef __CC_2500_H_
#define __CC_2500_H_

#include "board.h"
#include <msp430.h>

#define CC2500_IOCFG2      0x00      /*  IOCFG2   - GDO2 output pin configuration  */
#define CC2500_IOCFG1      0x01      /*  IOCFG1   - GDO1 output pin configuration  */
#define CC2500_IOCFG0      0x02      /*  IOCFG1   - GDO0 output pin configuration  */
#define CC2500_FIFOTHR     0x03      /*  FIFOTHR  - RX FIFO and TX FIFO thresholds */
#define CC2500_SYNC1       0x04      /*  SYNC1    - Sync word, high byte */
#define CC2500_SYNC0       0x05      /*  SYNC0    - Sync word, low byte */
#define CC2500_PKTLEN      0x06      /*  PKTLEN   - Packet length */
#define CC2500_PKTCTRL1    0x07      /*  PKTCTRL1 - Packet automation control */
    #define CC2500_PKTCTRL0    0x05      /*  PKTCTRL0 - Packet automation control */
#define CC2500_ADDR        0x09      /*  ADDR     - Device address */
#define CC2500_CHANNR      0x0A      /*  CHANNR   - Channel number */
#define CC2500_FSCTRL1     0x0B      /*  FSCTRL1  - Frequency synthesizer control */
#define CC2500_FSCTRL0     0x0C      /*  FSCTRL0  - Frequency synthesizer control */
#define CC2500_FREQ2       0x0D      /*  FREQ2    - Frequency control word, high byte */
#define CC2500_FREQ1       0x0E      /*  FREQ1    - Frequency control word, middle byte */
#define CC2500_FREQ0       0x0F      /*  FREQ0    - Frequency control word, low byte */
#define CC2500_MDMCFG4     0x10      /*  MDMCFG4  - Modem configuration */
#define CC2500_MDMCFG3     0x11      /*  MDMCFG3  - Modem configuration */
    #define CC2500_MDMCFG2     0x13      /*  MDMCFG2  - Modem configuration */
#define CC2500_MDMCFG1     0x13      /*  MDMCFG1  - Modem configuration */
#define CC2500_MDMCFG0     0x14      /*  MDMCFG0  - Modem configuration */
#define CC2500_DEVIATN     0x15      /*  DEVIATN  - Modem deviation setting */
#define CC2500_MCSM2       0x16      /*  MCSM2    - Main Radio Control State Machine configuration */
    #define CC2500_MCSM1       0x3C      /*  MCSM1    - Main Radio Control State Machine configuration */
#define CC2500_MCSM0       0x18      /*  MCSM0    - Main Radio Control State Machine configuration */
#define CC2500_FOCCFG      0x19      /*  FOCCFG   - Frequency Offset Compensation configuration */
#define CC2500_BSCFG       0x1A      /*  BSCFG    - Bit Synchronization configuration */
#define CC2500_AGCCTRL2    0x1B      /*  AGCCTRL2 - AGC control */
#define CC2500_AGCCTRL1    0x1C      /*  AGCCTRL1 - AGC control */
#define CC2500_AGCCTRL0    0x1D      /*  AGCCTRL0 - AGC control */
#define CC2500_WOREVT1     0x1E      /*  WOREVT1  - High byte Event0 timeout */
#define CC2500_WOREVT0     0x1F      /*  WOREVT0  - Low byte Event0 timeout */
#define CC2500_WORCTRL     0x20      /*  WORCTRL  - Wake On Radio control */
#define CC2500_FREND1      0x21      /*  FREND1   - Front end RX configuration */
#define CC2500_FREND0      0x22      /*  FREDN0   - Front end TX configuration */
#define CC2500_FSCAL3      0x23      /*  FSCAL3   - Frequency synthesizer calibration */
#define CC2500_FSCAL2      0x24      /*  FSCAL2   - Frequency synthesizer calibration */
#define CC2500_FSCAL1      0x25      /*  FSCAL1   - Frequency synthesizer calibration */
#define CC2500_FSCAL0      0x26      /*  FSCAL0   - Frequency synthesizer calibration */
#define CC2500_RCCTRL1     0x27      /*  RCCTRL1  - RC oscillator configuration */
#define CC2500_RCCTRL0     0x28      /*  RCCTRL0  - RC oscillator configuration */
#define CC2500_FSTEST      0x29      /*  FSTEST   - Frequency synthesizer calibration control */
#define CC2500_PTEST       0x2A      /*  PTEST    - Production test */
#define CC2500_AGCTEST     0x2B      /*  AGCTEST  - AGC test */
#define CC2500_TEST2       0x2C      /*  TEST2    - Various test settings */
#define CC2500_TEST1       0x2D      /*  TEST1    - Various test settings */
#define CC2500_TEST0       0x2E      /*  TEST0    - Various test settings */

/* status registers */
#define CC2500_PARTNUM              0x30      /*  PARTNUM    - Chip ID */
#define CC2500_VERSION              0x31      /*  VERSION    - Chip ID */
#define CC2500_FREQEST              0x32      /*  FREQEST    – Frequency Offset Estimate from demodulator */
#define CC2500_LQI                  0x33      /*  LQI        – Demodulator estimate for Link Quality */
#define CC2500_RSSI                 0x34      /*  RSSI       – Received signal strength indication */
#define CC2500_MARCSTATE            0x35      /*  MARCSTATE  – Main Radio Control State Machine state */
#define CC2500_WORTIME1             0x36      /*  WORTIME1   – High byte of WOR time */
#define CC2500_WORTIME0             0x37      /*  WORTIME0   – Low byte of WOR time */
#define CC2500_PKTSTATUS            0x38      /*  PKTSTATUS  – Current GDOx status and packet status */
#define CC2500_VCO_VC_DAC           0x39      /*  VCO_VC_DAC – Current setting from PLL calibration module */
#define CC2500_TXBYTES              0x3A      /*  TXBYTES    – Underflow and number of bytes */
#define CC2500_RXBYTES              0x3B      /*  RXBYTES    – Overflow and number of bytes */
#define CC2500_RCCTRL1_STATUS       0x3C      /*  RCCTRL1_STATUS    – Last RC oscillator calibration result */
#define CC2500_RCCTRL0_STATUS       0x3D      /*  RCCTRL0_STATUS    – Last RC oscillator calibration result */

/* burst write registers */
#define CC2500_PA_TABLE0   0x3E      /*  PA_TABLE0 - PA control settings table */
#define CC2500_TXFIFO      0x3F      /*  TXFIFO  - Transmit FIFO */
#define CC2500_RXFIFO      0x3F      /*  RXFIFO  - Receive FIFO */

/* command strobe registers */
#define CC2500_SRES        0x30      /*  SRES    - Reset chip. */
#define CC2500_SFSTXON     0x31      /*  SFSTXON - Enable and calibrate frequency synthesizer. */
#define CC2500_SXOFF       0x32      /*  SXOFF   - Turn off crystal oscillator. */
#define CC2500_SCAL        0x33      /*  SCAL    - Calibrate frequency synthesizer and turn it off. */
#define CC2500_SRX         0x34      /*  SRX     - Enable RX. Perform calibration if enabled. */
#define CC2500_STX         0x35      /*  STX     - Enable TX. If in RX state, only enable TX if CCA passes. */
#define CC2500_SIDLE       0x36      /*  SIDLE   - Exit RX / TX, turn off frequency synthesizer. */
#define CC2500_SRSVD       0x37      /*  SRVSD   - Reserved.  Do not use. */
#define CC2500_SWOR        0x38      /*  SWOR    - Start automatic RX polling sequence (Wake-on-Radio) */
#define CC2500_SPWD        0x39      /*  SPWD    - Enter power down mode when CSn goes high. */
#define CC2500_SFRX        0x3A      /*  SFRX    - Flush the RX FIFO buffer. */
#define CC2500_SFTX        0x3B      /*  SFTX    - Flush the TX FIFO buffer. */
#define CC2500_SWORRST     0x3C      /*  SWORRST - Reset real time clock. */
#define CC2500_SNOP        0x3D      /*  SNOP    - No operation. Returns status byte. */


typedef struct{
    unsigned char LEN;          // packet length
    unsigned char ADDRESS;      // destination address
    unsigned char DATA[62];    // DATA
}CC_Protocol_t;


union{
    char B;
    struct
    {
        char FIFO_bytes_available  :4; //  The number of bytes available in the RX FIFO or free bytes in the TX FIFO
        char state                 :3; //  000 - IDLE state
                                       //  001 - RX (receive mode)
                                       //  010 - TX (Transmit mode)
                                       //  011 - FSTXON (Frequency synthesizer is on, ready to start transmitting)
                                       //  100 - CALIBRATE (Frequency synthesizer calibration is running)
                                       //  101 - SETTLING (PLL is settling)
                                       //  110 - RXFIFO_OVERFLOW (RX FIFO has overflowed. Read out any useful data, then flush the FIFO with SFRX)
                                       //  111 - TXFIFO_UNDERFLOW (TX FIFO has underflowed. Acknowledge with SFTX)
        char ready                 :1; //  0 = chip is ready
    };
}_cc2500_status;

enum CC_STATE{
    cc_IDLE             = 0,
    cc_RX               = 1,
    cc_TX               = 2,
    cc_FSTXON           = 3,
    cc_CALIBRATE        = 4,
    cc_SETTLING         = 5,
    cc_RXFIFO_OVERFLOW  = 6,
    cc_TXFIFO_OVERFLOW  = 7
};

void CC_Init();
int CC_GetRSSI();
void CC_WriteReg(unsigned char Reg, char RegVal);
void CC_ReadReg(unsigned char Reg, char* pRegVal);
void CC_Strobe(unsigned char Reg);
void CC_SetWaitState(unsigned char Reg, enum CC_STATE STATE);

void CC_SetAddress(unsigned char address);
void CC_SetChannel(unsigned char channr);

void CC_SendMessage(CC_Protocol_t MSG);
void CC_ReadMessage(CC_Protocol_t *pMSG);

void CC_GDO0Config(char signal);
void CC_GDO2Config(char signal);

char _debugData[8];
void _CC_DEBUG();

#endif //__CC_2500_H_
