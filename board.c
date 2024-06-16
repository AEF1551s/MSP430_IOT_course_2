#include <board.h>

char _buffer[11];

void initBoard()
{
    DCOCTL =  CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;

    if( CALBC1_16MHZ == 0xFF )// Do not run if calibration values are erased
    {
      volatile int i;
      P1DIR |= 0x03;
      P1OUT &= 0xFC; // turn led's off
      while(1) // endless ERROR loop
      {
        for(i = 0; i < 0x5FFF; i++);
        P1OUT ^= 0x03; // toggle
      }
    }
    _buffer[10] = 0;
    P1DIR = 0x03; // P1.7 .. P1.2 INPUT | P1.1 .. P1.0 OUTPUT (LED)
    P1DIR &= 0xFB;// make sure that P1.2 is input
    P1IFG &= 0b11111011;

    P1REN |= 0b00000100; // p1.2 enable pullup resistor
    P1OUT |= 0b00000100; // set P1.2 HIGH
    P2IFG = 0;
    P1IE |= 0b00000100; // enable interrupt
    P1IES |= 0b00000100; // high to low

    P2SEL &= 0b00111111; // clear SELECT bits on P2.6 & P2.7

    __asm(" EINT"); // enable Global interrupts


    P1OUT &= 0xFC; // turn led's off

    _status.cnt3 = 0;
    _status.cnt2 = 0;
    _status.ButtonSet = 0;
    _status.GDO_2_Set = 0;
    _status.GDO_0_Set = 0;
}

void SetLED(unsigned char led, unsigned char state);

/**
 * @brief Delay the given number of microseconds.
 *
 * @param us Number of microseconds to delay.
 */
void delay_us(unsigned int us)
{
    /*
     * 1 cycle @16MHz takes 62.5ns
     * 1us equals 16 CPU cycles (instructions)
     */
    __asm(" mov.w 0x0000(SP), R15   \n"
    "L1:    dec.w R15       \n" //
        "   NOP             \n"
        "   NOP             \n"
        "   NOP             \n"
        "   NOP             \n"
        "   NOP             \n"
        "   NOP             \n"
        "   NOP             \n"
        "   NOP             \n"
        "   NOP             \n"
        "   jnz L1          " // two CPU cycles
            );
}

void delay_ms(unsigned int ms)
{
    for(unsigned int i=0; i<ms; i++)
        delay_us(998);
}

void UART_Init()
{
    UCA0CTL0 = 0; // 8N1
    UCA0CTL1 = UCSSEL_2;    // SET clock source to SMCLK

    UCA0BR0 = (1666 & 0xFF);                            // set baud rate 9600
    UCA0BR1 = (1666 & 0xFF00) >> 8;                     // set baud rate 9600
    UCA0MCTL = 0x0C;     // set this bit according to reference manual 435.page
    IE2 = UCA0RXIE;       // enable interrupt on RXNE (when one byte is received)
    IFG2 = 0;               // clear all interrupt flags
    UCA0CTL1 &= ~UCSWRST; // enable UART module (exit from RESET state)

    P3DIR |= 0b00010000; // set UART TX line (P3.4) as OUTPUT and RX(P3.5) as INPUT
    P3SEL |= 0b00110000; // enable UART module take control over P3.4 and P3.5 LINEs
}

void UART_TX(char byte)
{
    while(UCA0STAT & UCBUSY); // check for ongoing communication
    UCA0TXBUF = byte;           // send BYTE
    while(UCA0STAT & UCBUSY); // wait till whole byte is sent
}

void SerialSendByte(char B)
{
    while(UCA0STAT & UCBUSY); // check for ongoing communication
   UCA0TXBUF = B;           // send BYTE
}

void SerialWrite(char *p)
{
    while(*p)
    {
        while(UCA0STAT & UCBUSY); // check for ongoing communication
        UCA0TXBUF = *p;           // send BYTE
        p++;
    }
}

void SerialWriteln(char *p)
{
    while(*p)
    {
        while(UCA0STAT & UCBUSY); // check for ongoing communication
        UCA0TXBUF = *p;           // send BYTE
        p++;
    }
    while(UCA0STAT & UCBUSY); // check for ongoing communication
    UCA0TXBUF = '\n';           // send BYTE
    while(UCA0STAT & UCBUSY); // check for ongoing communication
    UCA0TXBUF = '\r';           // send BYTE
}

char* ItoA(int val, char BASE)
{
    unsigned char i=10; // buffer length
    char neg = 0;
    if(val == 0)
    {
        _buffer[--i]='0';
        return &_buffer[i];
    }
    if(val<0 && BASE==10)
    {
        neg=1;
        val *= -1;
    }

    for(;val && i; val/=BASE)
        _buffer[--i] = "0123456789ABCDEF"[val%BASE];
    if(neg && BASE==10)_buffer[--i] = '-';
    if(BASE==2)
        while(i>2)
            _buffer[--i] = '0';

    return &_buffer[i];
}

void SPI_Init()
{
    P3DIR |= 0x0B; //
    P3OUT |= 0x01; // set P3.0 high

    UCB0CTL1 = UCSWRST;
    UCB0CTL1 = UCSWRST | UCSSEL1;
    UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC;; // MSB first

    UCB0BR0 = 8;// form 16Mhz/8 --> gives 2MHz spi clock speed
    UCB0BR1 = 0;

    P3SEL |= 0x0E; // P3.7 .. P3.4 GPIO | P3.3 SPI CLK | P3.2 SPI MISO | P3.1 SPI MOSI | P3.0 GPIO (SPI CSN)

    UCB0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**

}

void SPI_Send(char data)
{
    IFG2 &= ~UCB0RXIFG;
    UCB0TXBUF = data;
    while(!(IFG2 & UCB0RXIFG));
}

void SPI_Read(char *pData)
{
    *pData = UCB0RXBUF;
}
