/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "unistd.h"
#include "system.h"
#include "alt_types.h"
#include "altera_avalon_uart_regs.h"
#include "sys\alt_irq.h"

alt_u8 txdata = 0;	//send
alt_u8 rxdata = 0;	//receive

// 串口中断服务函数
void IRQ_UART_Send(unsigned char recv){

	//rxdata = IORD_ALTERA_AVALON_UART_RXDATA(UART_BASE);
	txdata = recv;

	// judge if TRDY(T ready) is true
	while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BASE)&
			ALTERA_AVALON_UART_STATUS_TRDY_MSK)){
		;	//do nothing
	}

	IOWR_ALTERA_AVALON_UART_TXDATA(UART_BASE, txdata);

}

void delayTime(uint32_t x){
    double temp = 12.87346;
    uint32_t i = 0;
    for(i = 0; i < x; i++){
        temp = sqrt(pow(98765.234, 0.5)+pow(temp, 0.8));
    }
}

void sendRGB(unsigned char R, unsigned char G, unsigned char B){
	IRQ_UART_Send(0x02);
	delayTime(10);
	IRQ_UART_Send(0x03);
	delayTime(10);
	IRQ_UART_Send(R);
	delayTime(10);
	IRQ_UART_Send(G);
	delayTime(10);
	IRQ_UART_Send(B);
	delayTime(10);
}

int main()
{

	//clear status register
	IOWR_ALTERA_AVALON_UART_STATUS(UART_BASE, 0);

	// write control register
	IOWR_ALTERA_AVALON_UART_CONTROL(UART_BASE, 0x80);	//enable receive ready interrupt


	unsigned char R = 0x40;
	unsigned char G = 0x50;
	unsigned char B = 0x9f;

	printf("Hello from NIOS II!\n");

	while(1){
		sendRGB(R, G, B);
		printf("R %u G %u B %u\n", R, G, B);
		R += 0x13;
		G += 0x40;
		B += 0x26;
	}

	return 0;
}
