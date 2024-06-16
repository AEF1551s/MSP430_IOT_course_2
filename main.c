#include <msp430.h>
#include "board.h"
#include "cc2500.h"


// Krisjanis Ivbulis    191RMC097
//Maris Vitols          211RDB174

/**
 * MAIN.c
 */
char RXbuf;

#define OWNADDRESS      50
#define DST_ADDRESS     51

//otram
//#define OWNADDRESS      51
//#define DST_ADDRESS     50

#define CHANNEL         231
void interrupt_init(){
    //Atljaut globalos interupts
    __bis_SR_register(GIE);
}

void timer_init(){
    //Stop timer before changing register
    //SMCLK source used by peripherals on Vcc = 3 is 16MHz
    //16e6 ticks per second  62.5 nanoseconds per tick
    //1e6 nanoseconds in milisecond -> nanoseconds in milisecond (1e6)/1 tick time in miliseconds (0.000625ms
    //16000 ticks in 1 milisecond
    //timer need to count 16000 times, to reach 1 milisecond
    //timer single cycle will be 16000/8 (clock divide) = 2000 times per ms)
    //TIMER A control register = TA source sel (SMCLK), (IDx = 3, divider 8), MC_1 count up to value in TACCR0 value SET WHEN START TIMER,

    TACCR0 = 0; //Stop timer
    TACCTL0 |= CCIE; //Enable interrupt for CCR0.
    TACTL |= TASSEL_2 | ID_3 | MC_1;//Select clock sources.set clock divider and mode. start counting
    //!!!Counts only to 65535
    //65535 cycles on 2Mhz frequency is 30ms
    //1sec =    32.7(timer interrupts) * 30ms -> around 33 times we need to count to have 1 second
    unsigned int count = 65535-1;
        TACCR0 = count;
}
unsigned int timer_interrupt_count = 0;
unsigned int laiks = 0;
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    interrupt_init();

    initBoard();

    SPI_Init();
    UART_Init();

    CC_Init();

    CC_SetAddress(OWNADDRESS); // unique address [1..254]
    CC_SetChannel(CHANNEL); // [0..253]

    CC_Protocol_t CC_TX, CC_RX;
    unsigned int i;

    //Clear data arrays
    for(i=0; i<sizeof(CC_TX)/sizeof(char); i++){
        CC_TX.DATA[i] = 0;
    }

    for(i=0; i<sizeof(CC_RX)/sizeof(char); i++){
         CC_RX.DATA[i] = 0;
     }

    //ieraksta vertibas
    CC_RX.LEN = 0;
    CC_RX.ADDRESS = 0;

    CC_TX.ADDRESS = DST_ADDRESS; // destination address

    CC_TX.DATA[0] = 'l';
    CC_TX.DATA[1] = 'a';
    CC_TX.DATA[2] = 'i';
    CC_TX.DATA[3] = 'k';
    CC_TX.DATA[4] = 's';
    CC_TX.DATA[5] = ':';
    CC_TX.DATA[6] = '2';


    CC_TX.LEN = 7; // 7 bytes to send

    //uzsaak laika atskaiti
    timer_init();

    while(1) // main while
    {
        //ascii table number 0 start from 48
        char tochar = (char)(laiks+48);
        CC_TX.DATA[6] = tochar;
        delay_ms(50);
        
        //Ja netiek gaiditi dati, iestata idle state, pec tam gaidisanas state
        //mainot state, tiek gaidits, kad to var izdarit, lai nepartrauktu esosho
        if(_cc2500_status.state != cc_RX)
        {
            CC_SetWaitState(CC2500_SIDLE, cc_IDLE);

            if(_cc2500_status.state == cc_RXFIFO_OVERFLOW)CC_Strobe(CC2500_SFRX);
            if(_cc2500_status.state == cc_TXFIFO_OVERFLOW)CC_Strobe(CC2500_SFTX);
            CC_SetWaitState(CC2500_SRX, cc_RX);
        }

        if(_status.GDO_2_Set)
        {
            _status.GDO_2_Set = 0;
        }

        //Ja dati tiek sanemti tie ir izvaditi ar UART seriala porta
        if(_status.GDO_0_Set)
        {
            //atiestat flagu
            _status.GDO_0_Set = 0;

            //Nolasa sanjemto zinju
            CC_ReadMessage(&CC_RX);

            //Ieraksta ar UART sanjemto zinju ar nedaudz formatesanu un paskaidrojumiem
            SerialWrite("Received ");
            SerialWrite(ItoA(CC_RX.LEN,10));
            SerialWrite(" bytes from: ");
            SerialWriteln(ItoA(CC_RX.ADDRESS,10));
            SerialWrite("DATA:\n\rHEX: ");
            for(i=0; i<CC_RX.LEN-1; i++)
            {
                SerialWrite(ItoA(CC_RX.DATA[i],16));
                SerialSendByte(' ');
            }
            SerialWrite("\n\rDEC: ");
            for(i=0; i<CC_RX.LEN-1; i++)
            {
                SerialWrite(ItoA(CC_RX.DATA[i], 10));
                SerialSendByte(' ');
            }
            SerialWrite("\n\r CHAR: ");
            for(i=0; i<CC_RX.LEN-1; i++)
                SerialSendByte(CC_RX.DATA[i]);

            P1OUT ^= 0x02; // toggle green led
            
            //Strobe ir ka viena baita instrukcijas prieksh cc2500, kuras sanjemot notiek kada automatiska darbiba mikroshema
            //katra strobe instrukcija, atbilst noteiktai darbibai
            CC_Strobe(CC2500_SFRX);
            CC_SetWaitState(CC2500_SRX, cc_RX);
        }

        //piespiezot pogu
        if(_status.ButtonSet)
        {
            _status.ButtonSet=0;
            //nosuta velamos datus uz cc2500 ar spi
            CC_SendMessage(CC_TX);
            
            CC_SetWaitState(CC2500_SRX, cc_RX);

            P1IFG &= 0xFB;// clear interrupt flag
            P1IE |= 0x04; // enable button interrupt
        }
        P3OUT |= 0x01;
        P1OUT ^= 0x01; // toggle red led
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void Port_1_ISR(void)
{
    P1IFG &= 0b11111011; // clear P1.2 interrupt pending flag
    _status.ButtonSet = 1;
}

#pragma vector=PORT2_VECTOR
__interrupt void Port_2_ISR(void)
{
    if(P2IFG & 0b10000000) // P2.7 line GDO_2
    {
        P2IFG &= 0b01111111; // clear P2.7 interrupt pending flag
        _status.GDO_2_Set = 1;
    }
    if(P2IFG & 0b01000000) // P2.6 line GDO_0
    {
        P2IFG &= 0b10111111; // clear P2.6 interrupt pending flag
        _status.GDO_0_Set = 1;
    }
}
//sanemsanas uartRX partraukums
#pragma vector = USCIAB0RX_VECTOR
__interrupt void UART_RX_ISR(void)
{
    if(IFG2 & 0x01) // check for RXNE flag
    {
        IFG2 &= 0xFE; // clear RXNE flag
        RXbuf = UCA0RXBUF;
    }
}

//TIMER INTERRUPT
#pragma vector = TIMERA0_VECTOR
__interrupt void TIMERA0_VECTOR_ISR(void)
{

    TACTL = MC_0; //Stop timer
    TACCTL0 &= ~CCIFG;
    TACTL |= TACLR;//Clear counter
    TACTL |= TASSEL_2 | ID_3 | MC_1;
    //count 33 times, to achieve 1 second interval.
    timer_interrupt_count++;
    timer_interrupt_count %=34;

    //when 1second has elapsed, increment laiks mainigais, in interval 0-9
    if(timer_interrupt_count == 33){
    laiks++;
    laiks%=10;
    //Iestata pogu, kad notiks datu nosutishana ar spi;
    _status.ButtonSet = 1;}
}







