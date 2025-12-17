#include"display_driver.h"
#include<string.h>
UC8 rfid_dat[13];
UC8 rfid_data[5][13] = {"4783264946382","4783264946382","4783264946382",
"4783264946382","4783264946382"}
UC8 byte_no = 0;
void check_rfid(void)
{
	IODIR0 = 12;// p0.2 and p0,3 as output pin

	UC8 check_flag = 0,i;

	for(i = 0;i<5;i++)
	{
		if(strcmp(rfid_dat,rfid_data[i])== NULL)
			{
				check_flag = 1;
				break;
			}
	}
	if(flag == 0)
	{
		LCD_INIT();
		LCD_STR("the card is wrong");
		delaym(2000);	
	}
	else
	{
		LCD_INIT();
		LCD_STR("the card is correct");
		delaym(100);
		//rotate the motor to one directiono
		IOSET0 = 8;
		IOCLR0 = 4;
		delaym(5000);// for 5 second
		//rotate the motor to another directiono
		IOSET0 = 4;
		IOCLR0 = 8;
		delaym(5000);
		// stop the motor
		IOCLR0 = 8;
		IOCLR0 = 4;

 
			

		
	}
	
}
void UART0_CONFIG(void){
U0LCR=0X83;
U0DLL=97;
U0DLM=0;
U0LCR=0X03;
}
void uart0(void) __irq
{
	UC8	temp = U0IIR;
	UC8 byte ;

	if(temp == 0x04)
		byte = U0RBR;
	if(byte_no<11)
		rfid_dat[byte_no++] = byte;
	if(byte_no == 11)
	{
		rfid_dat[11] = '\0';
		
		check_rfid();
	}
	VICVectAddr = 0;
		
}
int main()
{
	U32 count = 0;
	PINSEL0=0X5;
	UART0_CONFIG();
	VICIntSelect = 0;
	VICVectCntl0 = 0x20|6;
	VICVectAddr0 = (unsigned long)uart0; 

	LCD_INIT();
	LCD_CMD(0x80);
	LCD_STR("V25CE2A1");
	LCD_CMD(0XC0);
	LCD_STR("TECHACESS PRO");
	delaym(2000);
	U0IER = 1;
	VICIntEnable = 1<<6;
	while(1)
	{
		LCD_CMD(0x80);
		LCD_STR("WAVE THE CARD");
		count++;
	}

	
   

		
}