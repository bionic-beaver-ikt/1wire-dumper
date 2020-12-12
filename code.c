#define F_CPU 12000000UL
#include <avr/io.h>
#include <util/delay.h>

#define THERM_PORT PORTD
#define THERM_DDR DDRD
#define THERM_PIN PIND
#define THERM_DQ PD3
#define LED0 PD0
#define LED1 PD1
#define LED2 PD2

#define THERM_INPUT_MODE() THERM_DDR&=~(1<<THERM_DQ)
#define THERM_OUTPUT_MODE() THERM_DDR|=(1<<THERM_DQ)
#define THERM_LOW() THERM_PORT&=~(1<<THERM_DQ)

int main (void)
{
    int led0_bit=0;
    int led1_bit=0;
    
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(480);
	THERM_INPUT_MODE();
	_delay_us(60);
	int bit=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(420);

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
			int command[32];
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

	_delay_us(1);
    
void transmit(void)
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
    while (1)
    {
     init_receive();   
    }
}

    
