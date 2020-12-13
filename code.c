#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define THERM_PORT PORTD
#define THERM_DDR DDRD
#define THERM_PIN PIND
#define THERM_DQ PD3

#define LED0 PD0
#define LED1 PD1
#define LED2 PD2

#define Button1 PD2
#define Button2 PD3

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

int num_of_sign=6;
int command[32]={23,45,23,17,43,63,65,34,82,14,02,4,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
//command[]={2};

void show()
{
     while (num_of_sign>0)
	 //for (int times=0; times<128; times--)
	{
		PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));
        //sign (num_of_sign/10);
		if (((num_of_sign/10)%16/8)==1) PORTD|=(1<<DDI_D);
		if (((num_of_sign/10)%8/4)==1) PORTD|=(1<<DDI_C);
		if (((num_of_sign/10)%4/2)==1) PORTB|=(1<<DDI_B);
		if (((num_of_sign/10)%2)==1) PORTB|=(1<<DDI_A);
		PORTD|=(1<<DDI1);
        _delay_ms(2);
        PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));
        //sign (num_of_sign%10);
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
        //command[num_of_sign]/10;
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
        //command[num_of_sign]%10;
	}
}

ISR (TIMER1_COMPA_vect)
{
	cli();
	DDRD|=((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4)); //Порты сегментов на вывод
        //PORTD&=~((1<<DDI1)|(1<<DDI2)); //|(1<<DDI3)|(1<<DDI4));
    PORTD|=(1<<DDI3)|(1<<DDI4)|(1<<DDI1)|(1<<DDI2); //Порты сегментов на VCC (цифры погашены)
        
    DDRB|=((1<<DDI_A)|(1<<DDI_B)); //Порты данных на вывод
    DDRD|=((1<<DDI_C)|(1<<DDI_D)); //Порты данных на вывод
    PORTB&=~((1<<DDI_A)|(1<<DDI_B));
    PORTD&=~((1<<DDI_C)|(1<<DDI_D)); //0
	if(num_of_sign==2) num_of_sign=30;
	num_of_sign--;
	sei();
	show();
	_delay_ms(2000);
}        



int main (void)
{
    cli();
     TCCR1B |= (1<<WGM12); //CTC Mode
     TIMSK |= (1<<OCIE1A);
     //OCR1AH = 0xB7;
	 OCR1AH = 0x57;
     OCR1AL = 0x1B;
     TCCR1B |= (1<<CS12);
     
    /*DDRD|=((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
        PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
        
        DDRD|=((1<<DDI_A)|(1<<DDI_B));
        DDRB|=((1<<DDI_C)|(1<<DDI_D));
        PORTD&=~((1<<DDI_A)|(1<<DDI_B));
        PORTB&=~((1<<DDI_C)|(1<<DDI_D));
        
        _delay_ms(4800);*/
        
    int led0_bit=0;
    int led1_bit=0;
    
	
	/*THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(480);
	THERM_INPUT_MODE();
	_delay_us(60);*/
	int bit=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(420);

	sei();
	_delay_ms(2000);
    
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

void led1_blink()
{
    if (led1_bit==0)
    {
        THERM_DDR&=~(1<<THERM_DQ);
        led1_bit=1;
    }
    else
    {
        THERM_DDR|=(1<<THERM_DQ);
        led1_bit=0;
    }
}
    
void init_receive()
{
	THERM_LOW();
	THERM_INPUT_MODE();
	while (THERM_PIN&(1<<THERM_DQ)) {};
	_delay_us(480);
    
	for (int i=0; i<480; i++)
	{
		if(THERM_PIN&(1<<THERM_DQ))
		{
			i=480;
			_delay_us(55);
			THERM_OUTPUT_MODE();
			_delay_us(235);
			THERM_INPUT_MODE();
			_delay_us(190);
			while (THERM_PIN&(1<<THERM_DQ)) {};
			for (int j=0; j<32; j++)
			{
				for (int k=0; k<120; k++)
				{
					if(THERM_PIN&(1<<THERM_DQ))
					{
						if (k<15) command[j] = 1; else command[j] = 0;
						k=120;
					}
				_delay_us(1);
				}
			}
		}
	_delay_us(1);
	}
}

	//_delay_us(1);
    
void transmit()
{
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	if(bit) THERM_INPUT_MODE();
	_delay_us(60);
	THERM_INPUT_MODE();
	
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	THERM_INPUT_MODE();
	_delay_us(14);
	if(THERM_PIN&(1<<THERM_DQ)) bit=1;
	_delay_us(45);
}    

/*void sign(int num)
{

}*/


    while (1)
    {
		cli();
		PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));
		PORTD|=(1<<DDI_D);
		PORTD|=((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		_delay_ms(13333);
     //init_receive();
        //DDRD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
        
		
		//_delay_ms(500);
		//sei();
		//_delay_ms(2000);
		
        /*PORTD|=(1<<DDI_D); //8
        _delay_ms(500);
        PORTD&=~(1<<DDI1);
        PORTB|=(1<<DDI_A); //9
        _delay_ms(500);
        PORTB&=~(1<<DDI_A); //10
        PORTB|=(1<<DDI_B);
        _delay_ms(500);
        PORTB|=(1<<DDI_A); //11
        _delay_ms(500);
        PORTB&=~(1<<DDI_A);
        PORTB&=~(1<<DDI_B);
        PORTD|=(1<<DDI_C); //12
        _delay_ms(500);
        PORTB|=(1<<DDI_A); //13
        _delay_ms(500);
        PORTB&=~(1<<DDI_A); //14
        PORTB|=(1<<DDI_B);
        _delay_ms(500);
        PORTB|=(1<<DDI_A); //15
        _delay_ms(500);*/
		
    }
}

    
