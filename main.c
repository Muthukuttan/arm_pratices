#include"display_driver.h"
#include<string.h>
#define a 1<<12
#define b 1<<13

UC8 rfid_dat[14];
UC8 rfid_data[5][14] = {"478386494638","477326494638","478326494638",
"478326494637","478326494635"};

UC8 rfid_flag = 0;
UC8 byte_no = 0;
void check_rfid(void)
{
	UC8 check_flag = 0,i;
 // U0IER = 0; // disable UART0 interrupt during processing
	
	IODIR0 |=a|b;// p0.2 and p0,3 as output pin

	

	for(i = 0;i<5;i++)
	{
		if(strcmp(rfid_dat,rfid_data[i])== 0)
			{
				check_flag = 1;
				break;
			}
	}
	if(check_flag == 0)
	{
		LCD_INIT();
		LCD_STR("the card is wrong");
		LCD_CMD(0xc0);
		LCD_STR((UC8 *)rfid_dat);
		delaym(2000);	
		LCD_CMD(0x01);
	}
	else
	{
		LCD_INIT();
		LCD_CMD(0x80);
		LCD_STR("the card is correct");
		IODIR0 |= a|b;
		delaym(100);
		//rotate the motor to one directiono
		IOSET0 = a;
		IOCLR0 = b;
		delaym(5000);// for 5 second
		//rotate the motor to another directiono
		IOSET0 = b;
		IOCLR0 = a;
		delaym(5000);
		// stop the motor
		IOCLR0 = a;
		IOCLR0 = b;		
	}
	U0IER = 1; // enable UART0 interrupt after processing
}
void UART0_CONFIG(void){
U0LCR=0X83;
U0DLL=97;
U0DLM=0;
U0LCR=0X03;
}
/*void uart0(void) __irq
{
    
	UC8	temp = U0IIR;
	UC8 byte ;

	if(temp == 0x04)//if ((temp & 0x0E) == 0x04)
		byte = U0RBR;
	if(byte !='\r')
		rfid_dat[byte_no++] = byte;
	else
	{
		rfid_dat[byte_no] = '\0';
		
		rfid_flag = 1;
	}
	VICVectAddr = 0;
		
} */
int main( )
{
	U32 count = 0;
	PINSEL0=0X5;
	UART0_CONFIG();
	//VICIntSelect = 0;
	//VICVectCntl0 = 0x20|6;
	//VICVectAddr0 = (unsigned long)uart0; 

	LCD_INIT();
	LCD_CMD(0x80);
	LCD_STR("V25CE2A1");
	LCD_CMD(0XC0);
	LCD_STR("TECH ACESS PRO");
	delaym(2000);
	LCD_CMD(0x01);
	LCD_STR("WAVE THE CARD");

	U0IER = 1;
	//VICIntEnable = 1<<6;
	while(1)
	{
		while(((U0LSR)&1)== 0);
		rfid_dat[byte_no++]= U0RBR;
		if(rfid_dat[byte_no - 1]== '\r')
		{
			// check_rfid();
			LCD_CMD(0x01);
			LCD_CMD(0x80);
			LCD_STR(rfid_dat);
			delaym(5000);	
		}	
	//	LCD_CMD(0x80);
				//count++;
    //    if(rfid_flag== 1)
     //   {
       //     rfid_flag=0;
         //   check_rfid();
     }

}
