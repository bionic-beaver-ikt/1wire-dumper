/*
 * serial.c
 *
 * Created: 15.03.2019 20:54:16
 *  Author: Slava
 */ 

#define F_CPU 8000000UL
#define BAUD 9600L // Ñêîðîñòü îáìåíà äàííûìè
#define UBRRL_value (F_CPU/(BAUD*16))-1 //Ñîãëàñòíî çàäàííîé ñêîðîñòè ïîäñ÷èòûâàåì çíà÷åíèå äëÿ ðåãèñòðà UBRR
//#define UBRRL_value 25

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define THERM_PORT PORTC
#define THERM_DDR DDRC
#define THERM_PIN PINC
#define THERM_DQ PC4

/*#define SEG_A PB7
#define SEG_B PB6
#define SEG_C PB5
#define SEG_D PB4
#define SEG_E PB3
#define SEG_F PB2
#define SEG_G PB1
#define SEG_DOT PB0*/
#define SEG_1 PD6
#define SEG_2 PD1
#define SEG_3 PD0
#define SEG_4 PD5

/*#define DDI1_PORT PORTB
#define DDI1_DDR DDRB
#define DDI3_PORT PORTD
#define DDI3_DDR DDRD
#define DDI1_A PB0
#define DDI1_B PB1
#define DDI1_C PB2
#define DDI1_D PB3
#define DDI2_A PB4
#define DDI2_B PB5
#define DDI2_C PB6
#define DDI2_D PB7
#define Button1 PD2
#define Button2 PD3
#define DDI3_A PD4
#define DDI3_B PD5
#define DDI3_C PD6
#define DDI3_D PD7
#define AGC1 PC0
#define AGC2 PC1
#define DDI1_DOT PC2
#define DDI2_DOT PC3
#define DDI3_DOT PC4*/

#define LED_PORT PORTC
#define LED_DDR DDRC
#define LED1 PC2
#define LED2 PC0
#define LED3 PC4


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
//int rom[8];
uint8_t rom[8];
//int sign = 1;
//char temp;
//char temp2;
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
		LED1_ON();
		led1=1;
	}
	else
	{
		LED1_OFF();
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
	int code = 0;
	THERM_INPUT_MODE();
	_delay_us(10);
	while (THERM_PIN&(1<<THERM_DQ)) {}; //1
	_delay_us(470); //2

	for (int i=0; i<480; i++) //3
	{
		if(THERM_PIN&(1<<THERM_DQ)) //3a
		{
			i=480;
			_delay_us(55); //3b
			THERM_OUTPUT_MODE();
			_delay_us(235); //c
			THERM_INPUT_MODE();
			_delay_us(1); //(190)

			for (int j=0; j<8; j++)
			{
				while (THERM_PIN&(1<<THERM_DQ)) {};
				for (int kl=0; kl<100; kl++)
				{
					if (THERM_PIN&(1<<THERM_DQ))
                    {
						if (kl < 4)
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
						}
						kl=101;
					}
                    else _delay_us(1);
				}
			}
		}
	_delay_us(1);
	}
return code;
}

void transmit(int byte)
{
	THERM_INPUT_MODE();
	uint8_t i=8;
	while(i--){
		while (THERM_PIN&(1<<THERM_DQ)) {};
		THERM_OUTPUT_MODE();
		_delay_us(1);
		if (byte&1) THERM_INPUT_MODE();
		_delay_us(40);
		THERM_INPUT_MODE();
		byte>>=1;
	}
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

	LED_DDR |= (1<<LED1);
	LED_PORT |= (1<<LED1);
	//LED_DDR|= ((1<<LED1)|(1<<LED2)); //|(1<<LED3));
	DDRB=0xFF;
	DDRD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	PORTD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	//DDRB=0xFF;
	PORTB=0xFF;
	PORTD&=~((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	_delay_ms(1000);

num_of_sign=4;
OCR1AH = 0x27;
rom[0]=0x01;
rom[1]=0x23;
rom[2]=0x45;
rom[3]=0x67;
rom[4]=0x89;
rom[5]=0xAB;
rom[6]=0xCD;
rom[7]=0xEF;
sei();
_delay_ms(6000);
code[0]=16;
code[1]=0x0e;
code[2]=0x0a;
code[3]=0x0d;
while(1)
{
	//cli();
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);
	lamp();
	_delay_ms(100);

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

/*while (therm_reset()) {} ;
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
rom[7]=therm_read_byte();*/
segment=0;
OCR1AH = 0xB7;
num_of_sign=4;
TIMSK |= (1<<OCIE1A);
_delay_ms(300);
sei();
_delay_ms(2000);
//cli();
code[0]=0;
code[1]=19;
code[2]=0x0e;
code[3]=17;
_delay_ms(2000);

rom[0]=0x00;
rom[1]=0x01;
rom[2]=0x02;
rom[3]=0x03;
rom[4]=0x04;
rom[5]=0x05;
rom[6]=0x06;
rom[7]=0x07;

for (int ifg=0; ifg<10; ifg ++)
{
	num_of_sign=4;
	while (init_receive() == 0x33) {}
	transmit(rom[0]);
	transmit(rom[1]);
	transmit(rom[2]);
	transmit(rom[3]);
	transmit(rom[4]);
	transmit(rom[5]);
	transmit(rom[6]);
	transmit(rom[7]);
	_delay_ms(500);
}

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
