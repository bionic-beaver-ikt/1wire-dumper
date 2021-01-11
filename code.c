#define F_CPU 12000000UL
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

int main (void)
{
    cli();
     TCCR1B |= (1<<WGM12); //CTC Mode
     TIMSK |= (1<<OCIE1A);
     OCR1AH = 0xD7;
	 //OCR1AH = 0x57;
     OCR1AL = 0x1B;
     TCCR1B |= (1<<CS12);
    int led0_bit=0;

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
		if (!(byte&1)) THERM_OUTPUT_MODE();
		_delay_us(16);
		THERM_INPUT_MODE();
		byte>>=1;
	}
}

    while (1)
    {
		cli();
        num_of_sign=8;
        THERM_LOW();

		PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));

        DDRB|=(1<<PB3);
        PORTB|=(1<<PB3);
		_delay_ms(1000);
		PORTB&=~(1<<PB3);
		if (init_receive() == 0x33)
		{
			command[3]=73;
			transmit(0x12);
			transmit(0x34);
			transmit(0x56);
			transmit(0x78);
			transmit(0x9A);
			transmit(0xBC);
			transmit(0xDE);
			transmit(0xF0);
		}
		sei();
		_delay_ms(1000);
		cli();
		PORTB|=(1<<PB3);
		_delay_ms(1000);
    }
}
