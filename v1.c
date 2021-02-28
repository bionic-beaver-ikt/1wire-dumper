/*
 * serial.c
 *
 * Created: 15.03.2019 20:54:16
 *  Author: Slava
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define THERM_PORT PORTC
#define THERM_DDR DDRC
#define THERM_PIN PINC
#define THERM_DQ PC4

#define SEG_1 PD6
#define SEG_2 PD1
#define SEG_3 PD0
#define SEG_4 PD5

#define LED_PORT PORTC
#define LED_DDR DDRC
#define LED1 PC0
#define LED2 PC1
#define LED3 PC2

#define THERM_INPUT_MODE() THERM_DDR&=~(1<<THERM_DQ)
#define THERM_OUTPUT_MODE() THERM_DDR|=(1<<THERM_DQ)
#define THERM_LOW() THERM_PORT&=~(1<<THERM_DQ)
#define THERM_HIGH() THERM_PORT|=(1<<THERM_DQ)

#define LED1_ON() LED_PORT|=(1<<LED1)
#define LED2_ON() LED_PORT|=(1<<LED2)
#define LED3_ON() LED_PORT|=(1<<LED3)
#define LED1_OFF() LED_PORT&=~(1<<LED1)
#define LED2_OFF() LED_PORT&=~(1<<LED2)
#define LED3_OFF() LED_PORT&=~(1<<LED3)

int seg[20] = {175,12,181,157,30,155,187,13,191,159,63,186,163,188,179,51,48,178,56,55}; //r,n,t,p
int led1=1;
int num_of_sign=4;
int code[4]={1,2,3,4};
uint8_t rom[8];
int segment = 0;
uint8_t temperature[2];
int8_t digit;
uint16_t decimal;

ISR (TIMER1_COMPA_vect)
{
    cli();
    num_of_sign--;
	code[0]=(rom[(3-num_of_sign)*2]&0xF0)>>4;
	code[1]=(rom[(3-num_of_sign)*2]&0x0F);
	code[2]=(rom[(3-num_of_sign)*2+1]&0xF0)>>4;
	code[3]=(rom[(3-num_of_sign)*2+1]&0x0F);
	if(num_of_sign<=0) TIMSK &= ~(1<<OCIE1A);
	sei();
}

ISR (TIMER0_OVF_vect)
{
	cli();
	PORTD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	switch (segment)
	{
		case 0:
		PORTB= seg[(code[0])];
		PORTD&=~(1<<SEG_1);
		break;
		case 1:
		PORTB= seg[(code[1])];
		PORTD&=~(1<<SEG_2);
		break;
		case 2:
		PORTB= seg[(code[2])];
		PORTD&=~(1<<SEG_3);
		break;
		case 3:
		PORTB= seg[(code[3])];
		PORTD&=~(1<<SEG_4);
		segment=-1;
		break;
	}
	segment++;
	sei();
}

void lamp()
{
	if (led1!=1)
	{
		LED3_ON();
		led1=1;
	}
	else
	{
		LED3_OFF();
		led1=0;
	}
}
uint8_t therm_reset(){
	uint8_t i;
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(480);
	THERM_INPUT_MODE();
	_delay_us(60);
	i=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(420);
	return i;
}

void therm_write_bit(uint8_t bit){
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1); //1mks!!!
	if(bit) THERM_INPUT_MODE();
	_delay_us(60);
	THERM_INPUT_MODE();
}

uint8_t therm_read_bit(void){
	uint8_t bit=0;
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	THERM_INPUT_MODE();
	_delay_us(14);
	if(THERM_PIN&(1<<THERM_DQ)) bit=1;
	_delay_us(45);
	return bit;
}

uint8_t therm_read_byte(void){
	uint8_t i=8, n=0;
	while(i--){
		n>>=1;
		n|=(therm_read_bit()<<7);
	}
	return n;
}

void therm_write_byte(uint8_t byte){
	uint8_t i=8;
	while(i--){
		therm_write_bit(byte&1);
		byte>>=1;
	}
}

int init_receive()
{
	cli();
	int code = 0;
	THERM_INPUT_MODE();
	_delay_us(1);
	while (!(THERM_PIN&(1<<THERM_DQ))) {};
	while (THERM_PIN&(1<<THERM_DQ)) {}; //1
	_delay_us(475); //2
	
	for (int i=0; i<480; i++) //3
	{
		if(THERM_PIN&(1<<THERM_DQ)) //3a
		{
			i=480;
			_delay_us(55); //3b
			THERM_OUTPUT_MODE();
			_delay_us(235); //c
			THERM_INPUT_MODE();
			_delay_us(100); //(190)
			for (int j=0; j<8; j++)
			{
				while (THERM_PIN&(1<<THERM_DQ)) {};
				for (int kl=0; kl<20; kl++)
				{
					if (THERM_PIN&(1<<THERM_DQ))
                    {
						switch (j)
						{
							case 0:
							code += 1;
							break;
							case 1:
							code += 2;
							break;
							case 2:
							code += 4;
							break;
							case 3:
							code += 8;
							break;
							case 4:
							code += 16;
							break;
							case 5:
							code += 32;
							break;
							case 6:
							code +=64;
							break;
							case 7:
							code += 128;
							break;
						}
						kl=110;
					}
                    //_delay_us(1);
					else asm("nop");
					//asm("nop");
					//asm("nop");
					//asm("nop");

				}
				if (j!=7) while (!(THERM_PIN&(1<<THERM_DQ))) {};
			}
		}
	 else _delay_us(1);
	}
	uint8_t i=8;
if (code == 0x33) PORTC |= (1<<PC0); else PORTC |= (1<<PC1);
return code;
}

void transmit(int byte)
{
	PORTC|=(1<<PC3);
	THERM_INPUT_MODE();
	uint8_t i=8;
	while (!(THERM_PIN&(1<<THERM_DQ))) {};
	while(i--)
	{
		while (THERM_PIN&(1<<THERM_DQ)) {};
		THERM_OUTPUT_MODE();
		_delay_us(1);
		if (byte&1) THERM_INPUT_MODE();
		_delay_us(17);
		THERM_INPUT_MODE();
		byte>>=1;
	}
	PORTC&=~(1<<PC3);
}

#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_DECIMAL_STEPS_12BIT 625 //.0625

int main(void)
{
	cli();
	TCCR0 |= (1<<CS00)|(1<<CS01); //prescaler 64
	TIMSK |= (1<<TOIE0); //overflow interrupt
    TIMSK |= (1<<OCIE1A); //output compare interrupt
	TCCR1B |= (1<<WGM12); //CTC Mode
	TCCR1B |= (1<<CS12); //prescaler 256
    OCR1AH = 0xB7;
    //OCR1AH = 0x57;
    OCR1AL = 0x1B;

	DDRB=0xFF;
	DDRD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	PORTD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	PORTB=0xFF;
	PORTD&=~((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	DDRC |= (1<<PC0)|(1<<PC1)|(1<<PC3);
	_delay_ms(100);

segment=0;
cli();
_delay_ms(100);
//sei();
//_delay_ms(2000);
//cli();
code[0]=0;
code[1]=19;
code[2]=0x0e;
code[3]=18;
OCR1AH = 0xB7;
num_of_sign=4;
TIMSK |= (1<<OCIE1A);
cli();


while(1)
{
	cli();	
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);
	//lamp();
	//_delay_ms(100);

//therm_reset();
//while (therm_reset()) {} ;
// therm_reset(); //!
//therm_write_byte(THERM_CMD_SKIPROM);
// therm_write_byte(THERM_CMD_READROM); //!
//num_of_sign=4;
//code[0]=17;
//code[1]=14;
//code[2]=5;
//code[3]=17;
//sei();
//_delay_ms(2000);
//cli();
//therm_reset();

while (therm_reset()) {} ;
cli();
therm_write_byte(THERM_CMD_READROM);
OCR1AH = 0xB7;
num_of_sign=4;
rom[0]=therm_read_byte();
rom[1]=therm_read_byte();
rom[2]=therm_read_byte();
rom[3]=therm_read_byte();
rom[4]=therm_read_byte();
rom[5]=therm_read_byte();
rom[6]=therm_read_byte();
rom[7]=therm_read_byte();


sei();
_delay_ms(5000);
cli();

code[0]=17;
code[1]=14;
code[2]=5;
code[3]=17;
sei();
_delay_ms(1000);
cli();

for (int ifg=0; ifg<10; ifg ++)
{
	num_of_sign=4;
	while (init_receive() != 0x33) {}
	//init_receive();
	transmit(rom[0]);
	transmit(rom[1]);
	transmit(rom[2]);
	transmit(rom[3]);
	transmit(rom[4]);
	transmit(rom[5]);
	transmit(rom[6]);
	transmit(rom[7]);
}

/*rom[0]=init_receive();
rom[1]=init_receive();
rom[2]=init_receive();
rom[3]=init_receive();
rom[4]=init_receive();
rom[5]=init_receive();
rom[6]=init_receive();
rom[7]=init_receive();*/
sei();	
_delay_ms(2000);
//}
cli();
lamp();
_delay_ms(500);
lamp();
_delay_ms(500);
lamp();
_delay_ms(500);
lamp();
_delay_ms(500);
lamp();
_delay_ms(500);
}
}
