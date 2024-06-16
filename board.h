#ifndef BOARD_H_
#define BOARD_H_

#include <msp430.h>

#define OWN_ADDRESS 51

struct{
    char GDO_0_Set          :1;
    char GDO_2_Set          :1;

    char ButtonSet          :1;
    char cnt2               :2;
    char cnt3               :3;
}_status;

void initBoard();

void UART_Init();
void UART_TX(char byte);
void SerialSendByte(char B);
void SerialWrite(char *p);
void SerialWriteln(char *p);
char* ItoA(int val, char BASE);

void SPI_Init();
void SPI_Send(char data);
void SPI_Read(char *pData);

void delay_us(unsigned int us);
void delay_ms(unsigned int ms);

#endif /* BOARD_H_ */
