#include "cc2500.h"

// Address Config = No address check
// Base Frequency = 2425.000000
// CRC Autoflush = false
// CRC Enable = true
// Carrier Frequency = 2425.749817
// Channel Number = 3
// Channel Spacing = 249.938965
// Data Format = Normal mode
// Data Rate = 249.939
// Device Address = 0
// Manchester Enable = false
// Modulated = true
// Modulation Format = MSK
// Packet Length = 255
// Packet Length Mode = Variable packet length mode. Packet length configured by the first byte after sync word
// Phase Transition Time = 1
// Preamble Count = 4
// RX Filter BW = 541.666667
// Sync Word Qualifier Mode = 30/32 sync word bits detected
// TX Power = 0
// Whitening = false
// PA table
static const char PA_TABLE[8]={0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//
// Rf settings for CC2500
//
static const unsigned char CC2500_RegVal[47] = {
        0x29,   //IOCFG2      GDO2Output Pin Configuration
        0x2E,   //IOCFG1      GDO1Output Pin Configuration
        0x06,   //IOCFG0      GDO0Output Pin Configuration
        0x07,   //FIFOTHR     RX FIFO and TX FIFO Thresholds
        0xD3,   //SYNC1       Sync Word, High Byte
        0x91,   //SYNC0       Sync Word, Low Byte
        0xFF,   //PKTLEN      Packet Length
        0x04,   //PKTCTRL1    Packet Automation Control
        0x05,   //PKTCTRL0    Packet Automation Control
        0x00,   //ADDR        Device Address
        0x00,   //CHANNR      Channel Number
        0x0B,   //FSCTRL1     Frequency Synthesizer Control
        0x00,   //FSCTRL0     Frequency Synthesizer Control
        0x5D,   //FREQ2       Frequency Control Word, High Byte
        0x93,   //FREQ1       Frequency Control Word, Middle Byte
        0xB1,   //FREQ0       Frequency Control Word, Low Byte
        0x68,   //MDMCFG4     Modem Configuration
        0x93,   //MDMCFG3     Modem Configuration
        0x93,   //MDMCFG2     Modem Configuration
        0x22,   //MDMCFG1     Modem Configuration
        0xF8,   //MDMCFG0     Modem Configuration
        0x50,   //DEVIATN     Modem Deviation Setting
        0x07,   //MCSM2       Main Radio Control State Machine Configuration
        0x3C,   //MCSM1       Main Radio Control State Machine Configuration
        0x18,   //MCSM0       Main Radio Control State Machine Configuration
        0x16,   //FOCCFG      Frequency Offset Compensation Configuration
        0x6C,   //BSCFG       Bit Synchronization Configuration
        0x43,   //AGCCTRL2    AGC Control
        0x40,   //AGCCTRL1    AGC Control
        0x91,   //AGCCTRL0    AGC Control
        0x87,   //WOREVT1     High Byte Event0 Timeout
        0x6B,   //WOREVT0     Low Byte Event0 Timeout
        0xF8,   //WORCTRL     Wake On Radio Control
        0x56,   //FREND1      Front End RX Configuration
        0x10,   //FREND0      Front End TX configuration
        0xA9,   //FSCAL3      Frequency Synthesizer Calibration
        0x0A,   //FSCAL2      Frequency Synthesizer Calibration
        0x00,   //FSCAL1      Frequency Synthesizer Calibration
        0x11,   //FSCAL0      Frequency Synthesizer Calibration
        0x41,   //RCCTRL1     RC Oscillator Configuration
        0x00,   //RCCTRL0     RC Oscillator Configuration
        0x59,   //FSTEST      Frequency Synthesizer Calibration Control
        0x7F,   //PTEST       Production Test
        0x3F,   //AGCTEST     AGC Test
        0x88,   //TEST2       Various Test Settings
        0x31,   //TEST1       Various Test Settings
        0x0B,   //TEST0       Various Test Settings
};

void _CC_FifoAccess(unsigned char address, char* pData, unsigned char len);

void CC_Init()
{
    P2DIR &= 0x3F;// set P2.6 & P2.7 lines as INPUT
    P2OUT &= 0x3F;

    /* Radio power up reset*/
    /* pulse CSn low then high */
    unsigned char CCi;
    P3OUT &= 0xFE; // set P3.0 low
    delay_us(500);
    P3OUT |= 0x01;
    delay_us(500);


    P3OUT &= 0xFE; /* pull CSn low and wait for SO to go low */
    while(P3IN & 0x04);
    SPI_Send(CC2500_SRES);
    while(!(P3IN & 0x04));// wait for SO to go HI
    while(P3IN & 0x04);/* wait for SO to go low again, reset is complete at that point */

    P3OUT |= 0x01;
    delay_us(200);

    unsigned char PartNum = 0; // read SPI
    CC_ReadReg(CC2500_PARTNUM | 0xC0, &PartNum);
    while(PartNum != 0x80); // check for right CC2500 PartNum value

    /* initialize radio registers */
    P3OUT &= 0xFE; // set P3.0 low
    while(P3IN & 0x04);
    SPI_Send(CC2500_IOCFG2 | 0x40); // send first register address with burst bit set
    for(CCi=0; CCi<47; CCi++)
    {
        SPI_Send(CC2500_RegVal[CCi]);
    }
    P3OUT |= 0x01;
    delay_us(500);

    P3OUT &= 0xFE; // set P3.0 low
    while(P3IN & 0x04);
    SPI_Send(CC2500_PA_TABLE0);
    for(CCi=0; CCi<8; CCi++)
    {
        SPI_Send(PA_TABLE[CCi]);
    }
    P3OUT |= 0x01;

    char s=0;

    CC_SetWaitState(CC2500_SIDLE, cc_IDLE);
//    CC_SetWaitState(CC2500_SRX, cc_RX);
//    CC_GDO2Config(0x0A);// PLL lock detector  ( default is 0x29)

    delay_us(500);

    // force FS calibration
//    CC_WriteReg(CC2500_MCSM2, 0x03);
//    CC_WriteReg(CC2500_WOREVT1, 0x03); // EVENT0=800
//    CC_WriteReg(CC2500_WOREVT0, 0x20);
//    CC_WriteReg(CC2500_WORCTRL, 0x38);// EVENT1 = 3; RC_Cal=1; WOR_res=0
//    delay_us(3000);
//    CC_WriteReg(CC2500_WORCTRL, 0x30); // RC_Cal=0;
    {
        char RCC1, RCC0;
        CC_ReadReg(CC2500_RCCTRL1_STATUS, &RCC1);
        CC_ReadReg(CC2500_RCCTRL0_STATUS, &RCC0);
        CC_WriteReg(CC2500_RCCTRL1, RCC1);
        CC_WriteReg(CC2500_RCCTRL0, RCC0);
    }

    P3OUT |= 0x01;

    // prepare interrupt lines

    P2IES &= 0b00111111; // Interrupt on LOW-to-HIGH
    P2IFG &= 0x3F; //
    P2IE |= 0b11000000; // enable interrupts

    _status.GDO_0_Set = 0;
    _status.GDO_2_Set = 0;
}

int CC_GetRSSI() // datasheet page nr 35
{
    char s=0;
    CC_ReadReg(CC2500_RSSI | 0xC0, &s);// read RX bytes

    int val=0;
    if(s & 0x80)
    {
        val = ((s-256)/2 - 72); // RSSI_offset 2.4kB=71    10kB=69     250kB=72    500kB=72
    }
    else
    {
        val = ((s/2)-72);
    }
    return val;
}

void CC_WriteReg(unsigned char Reg, char RegVal)
{
    char rdata=0;
    unsigned char retries = 10;

    do{
        P3OUT &= 0xFE; // set CC2500 CSN low
        while(P3IN & 0x04);
         SPI_Send(Reg);
         SPI_Send(RegVal);
        P3OUT |= 0x01; // set CC2500 CSN high

        delay_us(100);
        P3OUT &= 0xFE; // set CC2500 CSN low
        while(P3IN & 0x04);
         SPI_Send(Reg|0x80); // read reg value
         _cc2500_status.B = UCB0RXBUF;
         SPI_Send(0);
         SPI_Read(&rdata);
        P3OUT |= 0x01; // set CC2500 CSN high
        retries--;
    }while(retries && rdata!=RegVal);
}

void CC_ReadReg(unsigned char Reg, char* pRegVal)
{
    P3OUT &= 0xFE; // set CC2500 CSN low
    while(P3IN & 0x04);
    SPI_Send(Reg | 0x80);
    _cc2500_status.B = UCB0RXBUF;
    SPI_Send(0);
    SPI_Read(pRegVal);

    P3OUT |= 0x01; // set CC2500 CSN high
}

void CC_Strobe(unsigned char Reg)
{
    if(Reg<0x30)return;

    P3OUT &= 0xFE; // set CC2500 CSN low
    while(P3IN & 0x04);
    SPI_Send(Reg);
    _cc2500_status.B = UCB0RXBUF;
    P3OUT |= 0x01; // set CC2500 CSN high
}

void CC_SetWaitState(unsigned char Reg, enum CC_STATE STATE)
{
    if(Reg<0x30)return;
    char r;
    char rr=10;
    do{
        r=10;
        P3OUT &= 0xFE; // set CC2500 CSN low
        while(P3IN & 0x04);

        SPI_Send(Reg);
        do{
            SPI_Send(CC2500_SNOP);
            _cc2500_status.B = UCB0RXBUF;
            r--;
        }while(_cc2500_status.state != STATE && r);

        P3OUT |= 0x01; // set CC2500 CSN high
        rr--;
    }while(rr>0 && !r);
}

void CC_SetAddress(unsigned char address)
{
    if(address==0 || address==255)
        return;

    CC_WriteReg(CC2500_ADDR, address); // 0x09
    delay_us(500); // delay

    char add = 0;
    CC_ReadReg(CC2500_ADDR, &add);// send out Dummy Byte to read data out from CC2500
    if(add != address) // check
        while(1);
}

void CC_SetChannel(unsigned char channr)
{
    CC_SetWaitState(CC2500_SIDLE, cc_IDLE);
    delay_ms(2); // delay
    CC_WriteReg(CC2500_CHANNR, channr);
    delay_ms(2); // delay
    CC_Strobe(CC2500_SCAL);
    delay_ms(2); // delay
}

void CC_SendMessage(CC_Protocol_t MSG)
{
    unsigned int len;
    P2IE &= 0xBF;

    CC_SetWaitState(CC2500_SIDLE, cc_IDLE);
    delay_us(100);

    CC_Strobe(CC2500_SFTX);  // flush the TX FIFO of any residual data
    len = MSG.LEN+2;
    MSG.LEN++;
    _CC_FifoAccess(CC2500_TXFIFO | 0x40, &MSG, len);

    CC_Strobe(CC2500_STX);  // Change state to TX, initiating data transfer
    while(!(P2IN & 0x40));  // Wait GDO0 to go HI -> sync TX'ed
    while(P2IN & 0x40);     // Wait GDO0 to clear -> end of pkt

    P2IFG &= 0xBF;
    P2IE |= 0x40;
}

void CC_ReadMessage(CC_Protocol_t *pData)
{
    char bytestoread, tmp;
    char buff[64];
    int i;

    P2IE &= 0x3F;  // disable interrupts on GDO..

    CC_ReadReg(CC2500_RXBYTES | 0xC0, &bytestoread);

    delay_us(10);

    i=0;
    while(1)
    {
        CC_ReadReg(CC2500_RXBYTES | 0xC0, &tmp);

        if(bytestoread != tmp)
        {
            bytestoread = tmp;
            i=0;
        }
        else if(i<3)
            i++;
        else
            break;
    }

    if(bytestoread)
        _CC_FifoAccess(CC2500_RXFIFO | 0xC0, pData, bytestoread);

    P2IFG &= 0x3F;  // clear interrupt flags
    P2IE |= 0xC0;  // enable interrupts on GDO..
}

void CC_GDO0Config(char signal)
{
    P3OUT &= 0xFE; // set CC2500 CSN low
    while(P3IN & 0x04);
    SPI_Send(CC2500_IOCFG0);
    SPI_Send(signal);
    P3OUT |= 0x01; // set CC2500 CSN high --> end of ...
}

void CC_GDO2Config(char signal)
{
    P3OUT &= 0xFE; // set CC2500 CSN low
    while(P3IN & 0x04);
    SPI_Send(CC2500_IOCFG2);
    SPI_Send(signal);
    P3OUT |= 0x01; // set CC2500 CSN high --> end of ...
}

void _CC_FifoAccess(unsigned char address, char* pData, unsigned char len)
{
    if(len==0 || !(address&0x40))
        return;
    P3OUT &= 0xFE; //
    while(P3IN & 0x04);
    P3OUT |= 0x01; // clear any SPI access
    do
    {
        P3OUT &= 0xFE;
        while(P3IN & 0x04);
        IFG2 &= ~UCB0RXIFG;
        UCB0TXBUF = address;
        while(!(IFG2 & UCB0RXIFG));
        do
        {
            IFG2 &= ~UCB0RXIFG;
            UCB0TXBUF = *pData;
            while(!(IFG2 & UCB0RXIFG));
            len--;
            if(address & 0x80)// if reading
            {
                *pData = UCB0RXBUF;
            }
            pData++;
            delay_us(5);
            if(P3IN & 0x01)// if communication was interrupted ...
                break;
        }while(len);
    }while(len);
    P3OUT |= 0x01;
}

void _CC_DEBUG()
{// debugging

    CC_ReadReg(CC2500_PKTSTATUS | 0xC0, &_debugData[0]);// burst read
    CC_ReadReg(CC2500_LQI       | 0xC0, &_debugData[1]);
    CC_ReadReg(CC2500_MARCSTATE | 0xC0, &_debugData[2]);
    CC_ReadReg(CC2500_TXBYTES   | 0xC0, &_debugData[3]);
    CC_ReadReg(CC2500_RXBYTES   | 0xC0, &_debugData[4]);

    char tmp1 = 0;
    char tmp2 = 0;

    CC_ReadReg(CC2500_CHANNR, &tmp1);
    CC_ReadReg(CC2500_ADDR, &tmp2);

    SerialWrite("CH  SRC  STATUS     PKTSTATUS  LQI  RSSI  MARCSTATE  TXBYTES  RXBYTES\r\n");

    SerialWrite(ItoA(tmp1, 10)); // channel
            SerialWrite("  ");
    SerialWrite(ItoA(tmp2, 10)); // address
            SerialWrite("   ");
    SerialWrite(ItoA(_cc2500_status.B, 2)); // STATUS
            SerialWrite("   ");
    SerialWrite(ItoA(_debugData[0], 2)); // PKTSTATUS
            SerialWrite("   ");
    SerialWrite(ItoA(_debugData[1], 10)); // LQI
            SerialWrite("    ");
    SerialWrite(ItoA(CC_GetRSSI(), 10)); // RSSI
            SerialWrite("   ");
    SerialWrite(ItoA(_debugData[2], 2));//MARCSTATE
            SerialWrite("    ");
    SerialWrite(ItoA(_debugData[3], 10)); // TXBYTES
            SerialWrite("          ");
    SerialWrite(ItoA(_debugData[4], 10)); // RXBYTES

    CC_ReadReg(CC2500_IOCFG0, &tmp1);
    SerialWrite("       # ");
    SerialWrite(ItoA(tmp1, 16)); // CC2500_IOCFG0
    CC_ReadReg(CC2500_IOCFG2, &tmp2);
    SerialWrite(" # ");
    SerialWrite(ItoA(tmp2, 16)); // CC2500_IOCFG2
    SerialWrite("\n\n\r");
}
