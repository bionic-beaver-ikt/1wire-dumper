#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define THERM_PORT PORTB
#define THERM_DDR DDRB
#define THERM_PIN PINB
#define THERM_DQ PB5

#define LED0 PD0

#define DDI_D PD6
#define DDI_C PD7
#define DDI_B PB0
#define DDI_A PB1
#define DDI4 PD0
#define DDI3 PD1
#define DDI2 PD2
#define DDI1 PD3

#define THERM_INPUT_MODE() THERM_DDR&=~(1<<THERM_DQ)
#define THERM_OUTPUT_MODE() THERM_DDR|=(1<<THERM_DQ)
#define THERM_LOW() THERM_PORT&=~(1<<THERM_DQ)

int num_of_sign=4;
int command[4]={0,0,0,0}; //={99,99,99,99,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,99};
//command[]={2};
   int led0_bit=0;
	
void show()
{
     while (num_of_sign>=0)
	{
		PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));
		if (((num_of_sign/10)%16/8)==1) PORTD|=(1<<DDI_D);
		if (((num_of_sign/10)%8/4)==1) PORTD|=(1<<DDI_C);
		if (((num_of_sign/10)%4/2)==1) PORTB|=(1<<DDI_B);
		if (((num_of_sign/10)%2)==1) PORTB|=(1<<DDI_A);
		PORTD|=(1<<DDI1);
        _delay_ms(2);
        PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));
		if (((num_of_sign%10)%16/8)==1) PORTD|=(1<<DDI_D);
		if (((num_of_sign%10)%8/4)==1) PORTD|=(1<<DDI_C);
		if (((num_of_sign%10)%4/2)==1) PORTB|=(1<<DDI_B);
		if (((num_of_sign%10)%2)==1) PORTB|=(1<<DDI_A);
		PORTD|=(1<<DDI2);
        _delay_ms(2);
        PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));
		if (((command[num_of_sign]/10)%16/8)==1) PORTD|=(1<<DDI_D);
		if (((command[num_of_sign]/10)%8/4)==1) PORTD|=(1<<DDI_C);
		if (((command[num_of_sign]/10)%4/2)==1) PORTB|=(1<<DDI_B);
		if (((command[num_of_sign]/10)%2)==1) PORTB|=(1<<DDI_A);
		PORTD|=(1<<DDI3);
        _delay_ms(2);
        PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));
		if (((command[num_of_sign]%10)%16/8)==1) PORTD|=(1<<DDI_D);
		if (((command[num_of_sign]%10)%8/4)==1) PORTD|=(1<<DDI_C);
		if (((command[num_of_sign]%10)%4/2)==1) PORTB|=(1<<DDI_B);
		if (((command[num_of_sign]%10)%2)==1) PORTB|=(1<<DDI_A);
		PORTD|=(1<<DDI4);
		_delay_ms(2);
	}
}

ISR (TIMER1_COMPA_vect)
{
	cli();
	DDRD|=((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4)); //Порты сегментов на вывод
    PORTD|=(1<<DDI3)|(1<<DDI4)|(1<<DDI1)|(1<<DDI2); //Порты сегментов на VCC (цифры погашены)
    DDRB|=((1<<DDI_A)|(1<<DDI_B)); //Порты данных на вывод
    DDRD|=((1<<DDI_C)|(1<<DDI_D)); //Порты данных на вывод
    PORTB&=~((1<<DDI_A)|(1<<DDI_B));
    PORTD&=~((1<<DDI_C)|(1<<DDI_D)); //0
	num_of_sign--;
	sei();
    show();
	if(num_of_sign<0) cli();
}

	
void led0_blink()
{
    if (led0_bit==0)
    {
        THERM_DDR&=~(1<<THERM_DQ);
        led0_bit=1;
    }
    else
    {
        THERM_DDR|=(1<<THERM_DQ);
        led0_bit=0;
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
	_delay_us(1);
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

int main (void)
{
    cli();
     TCCR1B |= (1<<WGM12); //CTC Mode
     TIMSK |= (1<<OCIE1A);
     //OCR1AH = 0xD7;
	 OCR1AH = 0x57;
     OCR1AL = 0x1B;
     TCCR1B |= (1<<CS12);

	PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
	PORTB&=~((1<<DDI_A)|(1<<DDI_B));
	PORTD&=~((1<<DDI_C)|(1<<DDI_D));
    THERM_LOW();
	_delay_ms(100);
	//DDRB|=(1<<PB5);
    while (1)
    {
		cli();
        num_of_sign=8;
		while (therm_reset()) {} ;
		//PORTB |= (1<<PB5);
		therm_write_byte(0x33);
		command[0]=therm_read_byte();
		command[1]=therm_read_byte();
		command[2]=therm_read_byte();
		command[3]=therm_read_byte();
		command[4]=therm_read_byte();
		command[5]=therm_read_byte();
		command[6]=therm_read_byte();
		command[7]=therm_read_byte();
		sei();
		_delay_ms(1000);
		cli();
    }
}
