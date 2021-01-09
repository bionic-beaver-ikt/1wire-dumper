#define F_CPU 12000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define THERM_PORT PORTB
#define THERM_DDR DDRB
#define THERM_PIN PINB
#define THERM_DQ PB5

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

int num_of_sign=32;
int command[32];//={99,99,99,99,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,99};
//command[]={2};

void show()
{
     while (num_of_sign>=0)
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
	num_of_sign--;
	if(num_of_sign<0)
    {
		sei();
        show();
		 cli();
		 //num_of_sign=32;
		 //_delay_ms(500);
    }
    else
    {
        sei();
        show();
    }
	//_delay_ms(2000);
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
     
    /*DDRD|=((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
        PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
        
        DDRD|=((1<<DDI_A)|(1<<DDI_B));
        DDRB|=((1<<DDI_C)|(1<<DDI_D));
        PORTD&=~((1<<DDI_A)|(1<<DDI_B));
        PORTB&=~((1<<DDI_C)|(1<<DDI_D));
        
        _delay_ms(4800);*/
        
    int led0_bit=0;
    int led1_bit=0;
    
	
void hren()
{
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(480);
	THERM_INPUT_MODE();
	_delay_us(60);
	int bit=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(420);
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
            
			for (int j=31; j>=0; j--)
			{
				command[j] = 0;
				while (THERM_PIN&(1<<THERM_DQ)) {};
				for (int kl=0; kl<100; kl++)
				{
					if (THERM_PIN&(1<<THERM_DQ))
                    {
                        command[j] = kl;
						/*if (kl<20)
                        {
                            command[j] = kl+1;
                            //PORTB&=~(1<<PB4);
                            //PORTB|=(1<<PB3);
                        }
                        else
                        {
                            command[j] = 0;
                            //PORTB&=~(1<<PB3);
                            //PORTB|=(1<<PB4);
                        }*/
						kl=101;
					}
                    else _delay_us(1);
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
	//if(bit) THERM_INPUT_MODE();
	_delay_us(60);
	THERM_INPUT_MODE();
	
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	THERM_INPUT_MODE();
	_delay_us(14);
	//if(THERM_PIN&(1<<THERM_DQ)) bit=1;
	_delay_us(45);
}    

/*void sign(int num)
{

}*/


    while (1)
    {
		cli();
        num_of_sign=32;
        THERM_LOW();
		
		PORTD&=~((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		PORTB&=~((1<<DDI_A)|(1<<DDI_B));
		PORTD&=~((1<<DDI_C)|(1<<DDI_D));
        
        DDRB|=(1<<PB3);  
        PORTB|=(1<<PB3);
		_delay_ms(10000); 
		PORTB&=~(1<<PB3);
		init_receive();
		sei();
		_delay_ms(1000);
		cli();
		PORTB|=(1<<PB3);
		//PORTD|=(1<<DDI_D);
		//PORTD|=((1<<DDI1)|(1<<DDI2)|(1<<DDI3)|(1<<DDI4));
		_delay_ms(1000);
		
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

    
