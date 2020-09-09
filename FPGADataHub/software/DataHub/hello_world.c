
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>

#include "system.h"
#include "alt_types.h"
#include "altera_avalon_uart_regs.h"
#include "sys\alt_irq.h"
#include <stdio.h>

#include "Color_Qsys.h"
//#include "../ColorAnalysis_bsp/system.h"


alt_u8 txdata = 0;	//send
alt_u8 rxdata = 0;	//receive
unsigned int *RGBbase = COLOR_BASE;


// 串口发送
void IRQ_UART_Send(unsigned char data){

	//rxdata = IORD_ALTERA_AVALON_UART_RXDATA(UART_BASE);
	txdata = data;

	// judge if TRDY(T ready) is true
	while(!(IORD_ALTERA_AVALON_UART_STATUS(UART_BASE)&
			ALTERA_AVALON_UART_STATUS_TRDY_MSK)){
		;	//do nothing
	}

	IOWR_ALTERA_AVALON_UART_TXDATA(UART_BASE, txdata);

}

// 延时
void delayTime(uint32_t x){
    double temp = 12.87346;
    uint32_t i = 0;
    for(i = 0; i < x; i++){
        temp = sqrt(pow(98765.234, 0.5)+pow(temp, 0.8));
    }
}

// 发送RGB值
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



struct RGB GetColor(){
	struct RGB myRGB;
	while(1){
		unsigned int rawData = *RGBbase;
		myRGB.isValid = ((rawData>>31)==1);
		if(myRGB.isValid == 1){
			myRGB.BValue = (rawData & 0xFF);
			myRGB.GValue = ((rawData>>8) & 0xFF);
			myRGB.RValue = ((rawData>>16) & 0xFF);
			break;
		}
	}
	return myRGB;
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
		struct RGB myRGB = GetColor();
		//if(myRGB.isValid == 1){
		int red;
		int green;
		int blue;

		double dred = (double) myRGB.RValue;
		double dgreen = (double) myRGB.GValue;
		double dblue = (double) myRGB.BValue;

		//red = (int) (-0.0098*dred*dred+3.9431*dred-143.22);
		//red = (int) (-0.0062*dred*dred+3.1126*dred-131.16);
		red = (int) (-0.0259*dred*dred+6.2633*dred-115.91);


		//green = (int) (-0.008*dgreen*dgreen+3.7674*dgreen-166.82);
		//green = (int) (-0.006*dgreen*dgreen+3.2612*dgreen-145.38);
		green = (int) (-0.0266*dgreen*dgreen+6.767*dgreen-134.74);


		//blue = (int) (-0.0098*dblue*dblue+4.6305*dblue-291.92);
		//blue = (int) (-0.0004*dblue*dblue*dblue + 0.2215*dblue*dblue -33.719*dblue + 1689);
		if(dblue < 80){
			blue = (int) (-0.00162 *dblue*dblue + 5.3372*dblue - 163.11);
		}
		else{
			blue = 255;
		}

		R = (unsigned char) red;
		G = (unsigned char) green;
		B = (unsigned char) blue;

		if(red > 255)
			R = 255;
		else if(red < 0)
			R = 0;
		if(green > 255)
			G = 255;
		else if(green < 0)
			G = 0;
		if(blue > 255)
			B = 255;
		else if(blue < 0)
			B = 0;

		printf("dR %lf dG %lf dB %lf R %u G %u B %u\n", dred, dgreen, dblue, R, G, B);

		sendRGB(R, G, B);
	}

	return 0;
}




