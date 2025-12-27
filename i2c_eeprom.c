#include <LPC21xx.h>
#include <string.h>

/* ===================== TYPES ===================== */
typedef unsigned char u8;
typedef signed   char s8;
typedef unsigned short u16;
typedef signed   short s16;
typedef unsigned int  u32;
typedef signed   int  s32;
typedef float  f32;
typedef double f64;


/* ===================== I2C DEFINES ===================== */
// Pin function selection
#define SCL_EN  0x00000010   // P0.2
#define SDA_EN  0x00000040   // P0.3

// I2C speed configuration
#define CCLK      60000000
#define PCLK      (CCLK/4)
#define I2C_SPEED 100000
#define LOADVAL   ((PCLK/I2C_SPEED)/2)

// I2CONSET bits
#define   AA_BIT  2
#define   SI_BIT  3
#define  STO_BIT  4
#define  STA_BIT  5
#define I2EN_BIT  6


/* ===================== LED DEFINES ===================== */
#define gLED (1<<16)   // P1.16
#define rLED (1<<17)   // P1.17


/* ===================== FUNCTION PROTOTYPES ===================== */
/* delay */
void delay_ms(u32);

/* I2C low-level */
void init_i2c(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_restart(void);
void i2c_write(u8);
u8   i2c_nack(void);
u8   i2c_masterack(void);

/* EEPROM */
void i2c_eeprom_write(u8,u8,u8);
u8   i2c_eeprom_read(u8,u8);
void i2c_eeprom_page_write(u8,u8,u8*,u8);
void i2c_eeprom_seq_read(u8,u8,u8*,u8);


/* ===================== DELAY ===================== */
void delay_ms(u32 dlyMS)
{
    u32 j;
    for(; dlyMS > 0; dlyMS--)
    {
        for(j = 12000; j > 0; j--);
    }
}


/* ===================== I2C DRIVER ===================== */
void init_i2c(void)
{
    PINSEL0 = SCL_EN | SDA_EN;   // Enable SCL & SDA
    I2SCLL  = LOADVAL;
    I2SCLH  = LOADVAL;
    I2CONSET = 1 << I2EN_BIT;   // Enable I2C
}

void i2c_start(void)
{
    I2CONSET = 1 << STA_BIT;
    while(((I2CONSET >> SI_BIT) & 1) == 0);
    I2CONCLR = 1 << STA_BIT;
}

void i2c_restart(void)
{
    I2CONSET = 1 << STA_BIT;
    I2CONCLR = 1 << SI_BIT;//(if it happened before start)?
    while(((I2CONSET >> SI_BIT) & 1) == 0);
    I2CONCLR = 1 << STA_BIT;
}

void i2c_write(u8 dat)
{
    I2DAT = dat;
    I2CONCLR = 1 << SI_BIT;
    while(((I2CONSET >> SI_BIT) & 1) == 0);
}

void i2c_stop(void)
{
    I2CONSET = 1 << STO_BIT;
    I2CONCLR = 1 << SI_BIT;
}

u8 i2c_nack(void)
{
    I2CONSET = 0x00;      // NACK(why)?
    I2CONCLR = 1 << SI_BIT;
    while(((I2CONSET >> SI_BIT) & 1) == 0);
    return I2DAT;
}

u8 i2c_masterack(void)
{
    I2CONSET = 0x04;      // ACK
    I2CONCLR = 1 << SI_BIT;
    while(((I2CONSET >> SI_BIT) & 1) == 0);
    I2CONCLR = 0x04;
    return I2DAT;
}


/* ===================== EEPROM FUNCTIONS ===================== */
void i2c_eeprom_write(u8 slaveAddr, u8 wBuffAddr, u8 dat)
{
    i2c_start();
    i2c_write(slaveAddr << 1);   // SLA + W
    i2c_write(wBuffAddr);        // EEPROM address
    i2c_write(dat);              // Data
    i2c_stop();
    delay_ms(10);
}

u8 i2c_eeprom_read(u8 slaveAddr, u8 rBuffAddr)
{
    u8 dat;

    i2c_start();
    i2c_write(slaveAddr << 1);   // SLA + W
    i2c_write(rBuffAddr);

    i2c_restart();
    i2c_write((slaveAddr << 1) | 1);  // SLA + R

    dat = i2c_nack();
    i2c_stop();

    return dat;
}

void i2c_eeprom_page_write(u8 slaveAddr, u8 wBuffStartAddr, u8 *p, u8 nBytes)
{
    u8 i;

    i2c_start();
    i2c_write(slaveAddr << 1);
    i2c_write(wBuffStartAddr);

    for(i = 0; i < nBytes; i++)
        i2c_write(p[i]);

    i2c_stop();
    delay_ms(10);
}

void i2c_eeprom_seq_read(u8 slaveAddr, u8 rBuffStartAddr, u8 *p, u8 nBytes)
{
    u8 i;

    i2c_start();
    i2c_write(slaveAddr << 1);
    i2c_write(rBuffStartAddr);

    i2c_restart();
    i2c_write((slaveAddr << 1) | 1);

    for(i = 0; i < nBytes - 1; i++)
        p[i] = i2c_masterack();

    p[i] = i2c_nack();
    i2c_stop();
}


/* ===================== MAIN ===================== */
int main(void)
{
    u8 ch;

    init_i2c();
    IODIR1 = gLED | rLED;

    i2c_eeprom_write(0x50, 0x00, 'A');
    ch = i2c_eeprom_read(0x50, 0x00);

    delay_ms(100);

    if(ch == 'A')
        IOPIN1 = gLED;
    else
        IOPIN1 = rLED;

    while(1);
}
