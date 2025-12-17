typedef unsigned char UC8;
//typedef float F32;
//#include"lookuptable.c"
#include"delay.h"
#define LCD_D 0xff
#define RS 1<<8
#define E 1<<9
void LCD_INIT(void);
void LCD_CMD(UC8);
void LCD_DATA(UC8);
void LCD_STR(UC8* );
void LCD_INT(int);
void LCD_FLOAT(float);
//void CGRAM_WRITE(UC8);
//#include "display_driver.h"
void LCD_INIT(void)
{
    IODIR0 = LCD_D|RS|E;
    LCD_CMD(0x01);//to clear the screen
    LCD_CMD(0x02);// to set cursur to first position
    LCD_CMD(0x0c);// cursur off and lcd on
    LCD_CMD(0X38);// to select the 8 bit mode
	LCD_CMD(0X80);
}
void LCD_CMD(UC8 cmd)
{
    IOCLR0 = LCD_D;
    IOSET0 = cmd;
    IOCLR0 = RS ;
    IOSET0 = E;
    delaym(2);
    IOCLR0 = E;

}
void LCD_DATA(UC8 d)
{
    IOCLR0 = LCD_D;
    IOSET0 = d;
    IOSET0 = RS;
    IOSET0 = E;
    delaym(2);
    IOCLR0 = E;

}
void LCD_INT(int n)
{
	UC8 a[10] ;
	int i = 0;
	if(n==0)
		LCD_DATA(48);// to print zero
	else
	{
		if(n<0)
		{
			LCD_DATA('-');
			n = -n;
		}
		while(n)
		{
			a[i++] = n%10;
			n = n/10;
		}
		if(i==0)
			i = 2;
		for(i = i-1;i>=0;i--)
			LCD_DATA(48+a[i]);
	}
}
void LCD_STR(UC8* s)
{
	UC8 j = 0;
	while(*s)
	{
		LCD_DATA(*s++);
		j++;
		if(j == 16)
				LCD_CMD(0XC0);
		
	}
}
/*void CGRAM_WRITE(UC8 nbyte)
{
	UC8 i = 0;
	LCD_CMD(0X40);
	for( i = 0;i<nbyte;i++)
	LCD_DATA(cgram_lut[i]);
} */
void LCD_FLOAT(float f)
{
	int d;
	int i;
	if(f<0)
	{
		f = -f;
		LCD_DATA('-');
	}
	
	 i = (int)f;
	
  d = (int)((f-i)*1000.0);
	LCD_INT(i);
	LCD_DATA('.');
	LCD_INT(d);
}
	