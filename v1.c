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
#define THERM_DQ PC5

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

//int seg[16] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6,0xEE,0x3E,0x9C,0x7A,0x9E,0x8E};
int seg[20] = {1,2,4,8,16,155,32,64,128,0xF6,0xEE,0x3E,0x9C,0x7A,179,0x8E,0,178};
int a=1;
int num_of_sign=8;
int code[4]={1,2,3,4};
//int rom[8];
uint8_t rom[8];
int sign = 1;
char temp;
char temp2;

uint8_t temperature[2];
int8_t digit;
uint16_t decimal;



void seg_show()
{
     while (num_of_sign>=0)
        {
            PORTD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
			PORTB= seg[(code[0])];
			//PORTB= (code[0]);
            PORTD&=~(1<<SEG_1);
        _delay_ms(2);
			PORTD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
			PORTB= seg[(code[1])];
			//PORTB= (code[1]);
            PORTD&=~(1<<SEG_2);
        _delay_ms(2);
			PORTD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
			PORTB= seg[(code[2])];
			//PORTB= (code[2]);
            PORTD&=~(1<<SEG_3);
		_delay_ms(2);
			PORTD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
			PORTB= seg[(code[3])];
			//PORTB= (code[3]);
            PORTD&=~(1<<SEG_4);
			_delay_ms(2);
        }
}

ISR (TIMER1_COMPA_vect)
{
    cli();
    //PORTD&=~((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
    //PORTB=0xFF;
    num_of_sign--;
    sei();


/*code[0]=(rom[(7-num_of_sign)*2]&0xF0)>>4;
code[1]=(rom[(7-num_of_sign)*2]&0x0F);
code[2]=(rom[(7-num_of_sign)*2+1]&0xF0)>>4;
code[3]=(rom[(7-num_of_sign)*2+1]&0x0F);*/ //!

    seg_show();

if(num_of_sign<0) cli();
}

void lamp()
{
	if (a!=1)
	{
		//PORTD |= 0b00001000;
		//PORTD &= 0b111110111;
		LED1_ON();
		a=1;
	}
	else
	{
		//PORTD |= 0b00001000;
		LED1_OFF();
		a=0;
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

#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc

#define THERM_DECIMAL_STEPS_12BIT 625 //.0625

//int test = 9;
//int test2 = 9;

void therm_read_temperature(char *buffer){
	while (therm_reset()) {} ;
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_CONVERTTEMP);
	lamp();
	while(!therm_read_bit());
	lamp();
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_RSCRATCHPAD);
	temperature[0]=therm_read_byte();
	temperature[1]=therm_read_byte();
	therm_reset();

	digit=temperature[0]>>4;
	digit|=(temperature[1]&0x7)<<4;
	temp = temperature[0];
	temp2 = temperature[1];

	//digit=temperature[0]>>1;
	decimal=temperature[0]&0xf;
	//digit;
	decimal*=62;
	sign = 1;

	if ((temperature[1]&0xF8) == 0xF8)
	{
		digit=127-digit;
		decimal= (1000-decimal)%1000;
		sign = 0;
	}
	//decimal*=THERM_DECIMAL_STEPS_12BIT;
	//sprintf(buffer, "%+d.%04u C", digit, decimal);
}


void init_USART() {
	UBRRL = UBRRL_value;       //Ìëàäøèå 8 áèò UBRRL_value
	UBRRH = (UBRRL_value) >> 8;  //Ñòàðøèå 8 áèò UBRRL_value
	UCSRB = (1<<TXEN);         //Áèò ðàçðåøåíèÿ ïåðåäà÷è
	//UCSRC = (1<< UCSZ0)|(1<< UCSZ1); //|(1<< URSEL) Óñòàíàâëèâåì ôîðìàò 8 áèò äàííûõ
	UCSRC = (1<< UCSZ0)|(1<< UCSZ1)|(1<< URSEL);
	//UCSRA = (1<<U2X); //Óäâîåíèå ñêîðîñòè
	//UCSRB = ( ( 1 << RXEN ) | ( 1 << TXEN ) ); //Ðàçðåøåíèå íà ïðèåì è íà ïåðåäà÷ó ÷åðåç USART
}

/*void DDI_show(int8_t digit, uint16_t decimal) {
	DDI1_PORT&=~(1<<DDI2_DOT);
	if (sign == 0) DDI1_PORT&=~(1<<DDI1_DOT); else DDI1_PORT|=(1<<DDI1_DOT);
	if ((digit/10)/8 != 1) DDI1_PORT&=~(1<<DDI1_D); else  DDI1_PORT|=(1<<DDI1_D);
	if ((digit/10)/4 != 1) DDI1_PORT&=~(1<<DDI1_C); else  DDI1_PORT|=(1<<DDI1_C);
	if ((digit/10)%4/2 != 1) DDI1_PORT&=~(1<<DDI1_B); else  DDI1_PORT|=(1<<DDI1_B);
	if ((digit/10)%2 != 1) DDI1_PORT&=~(1<<DDI1_A); else  DDI1_PORT|=(1<<DDI1_A);
	if ((digit%10)/8 != 1) DDI1_PORT&=~(1<<DDI2_D); else  DDI1_PORT|=(1<<DDI2_D);
	if ((digit%10)/4 != 1) DDI1_PORT&=~(1<<DDI2_C); else  DDI1_PORT|=(1<<DDI2_C);
	if ((digit%10)%4/2 != 1) DDI1_PORT&=~(1<<DDI2_B); else  DDI1_PORT|=(1<<DDI2_B);
	if ((digit%10)%2 != 1) DDI1_PORT&=~(1<<DDI2_A); else  DDI1_PORT|=(1<<DDI2_A);
	if ((decimal/100)/8 != 1) DDI3_PORT&=~(1<<DDI3_D); else  DDI3_PORT|=(1<<DDI3_D);
	if ((decimal/100)/4 != 1) PORTC&=~(1<<DDI3_C); else  PORTC|=(1<<DDI3_C); //!!!!!
	if ((decimal/100)%4/2 != 1) DDI3_PORT&=~(1<<DDI3_B); else  DDI3_PORT|=(1<<DDI3_B);
	if ((decimal/100)%2 != 1) DDI3_PORT&=~(1<<DDI3_A); else  DDI3_PORT|=(1<<DDI3_A);
}*/

void send_UART(char value) {
	while(!( UCSRA & (1 << UDRE)));   // Îæèäàåì êîãäà î÷èñòèòñÿ áóôåð ïåðåäà÷è
	UDR = value; // Ïîìåùàåì äàííûå â áóôåð, íà÷èíàåì ïåðåäà÷ó
}

int main(void)
{
	cli();
    TCCR1B |= (1<<WGM12); //CTC Mode
    TIMSK |= (1<<OCIE1A);
    OCR1AH = 0xB7;
    //OCR1AH = 0x57;
    OCR1AL = 0x1B;
    TCCR1B |= (1<<CS12);

	//LED_DDR|= ((1<<LED1)|(1<<LED2)); //|(1<<LED3));
	DDRB=0xFF;
	DDRD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	PORTD|=((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	//DDRB=0xFF;
	PORTB=0xFF;
	PORTD&=~((1<<SEG_1)|(1<<SEG_2)|(1<<SEG_3)|(1<<SEG_4));
	_delay_ms(1000);

//char buffer[15];
//uint8_t adc_value_h;
//uint8_t adc_value_l;
//uint16_t adc_value;
//int voltage = 0;

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

//therm_reset();
//while (therm_reset()) {} ;
// therm_reset(); //!
//therm_write_byte(THERM_CMD_SKIPROM);
// therm_write_byte(THERM_CMD_READROM); //!
num_of_sign=3;
code[0]=17;
code[1]=14;
code[2]=5;
code[3]=17;
sei();
_delay_ms(1000);
cli();
therm_reset();
therm_write_byte(THERM_CMD_READROM);
num_of_sign=4;
rom[0]=therm_read_byte();
rom[1]=therm_read_byte();
rom[2]=therm_read_byte();
rom[3]=therm_read_byte();
sei();
/*rom[0]=therm_read_byte();
rom[1]=therm_read_byte();
rom[2]=therm_read_byte();
rom[3]=therm_read_byte();*/ //!

//rom[4]=therm_read_byte();
//rom[5]=therm_read_byte();
//rom[6]=therm_read_byte();
//rom[7]=therm_read_byte();
/*for (int i=0; i<8; i++)
{
code[2*i]=(rom[i]&0xF0)>>4;
code[2*i+1]=(rom[i]&0x0F)>;
}
code[0]=(temperature[0]&0b11110000)>>4;
code[1]=temperature[0]&0b00001111;
code[2]=(temperature[1]&0b11110000)>>4;
code[3]=temperature[1]&0b00001111; */
//therm_reset();
/*code2[0]=therm_read_byte();
code2[1]=therm_read_byte();
code2[2]=therm_read_byte();
code2[3]=therm_read_byte();*/
//therm_read_temperature(buffer);
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

//_delay_ms(1000);

//DDI_show(digit, decimal);
//digit/10;

/*if (sign == 0) send_UART(0x2D);
else send_UART(0x2B);
if (digit > 9) send_UART(48+(digit/10));
send_UART(48+(digit%10));
send_UART(0x2E);
send_UART(48+(decimal/100));
send_UART(48+((decimal%100)/10));
send_UART(48+(decimal%10));*/
//lamp();
//ADMUX |= (1<<ADLAR); //|(1<<REFS0)|(1<<REFS1);//
/*ADMUX |= (1<<REFS0);
ADCSRA |= (1<<ADEN);
ADCSRA |= (1<<ADSC);
while (ADCSRA &  (1<<ADSC));
adc_value = ADCW;
send_UART(48+(adc_value/1000));
send_UART(48+(adc_value%1000/100));
send_UART(48+((adc_value%100)/10));
send_UART(48+(adc_value%10));
send_UART(0x2D);
voltage = 500.00/(1023/adc_value);
send_UART(48+(voltage/100));
send_UART(48+((voltage%100)/10));
send_UART(48+(voltage%10));*/
/*adc_value_h = ADCL;
//send_UART(48+(adc_value/1000));
send_UART(48+(adc_value_h/100));
send_UART(48+((adc_value_h%100)/10));
send_UART(48+(adc_value_h%10));*/
//send_UART(adc_value);
//adc_value_h = ADCH;
//send_UART(adc_value_h);
//send_UART(0x2D);
//adc_value_l = ADCL;
//send_UART(adc_value_l);
//_delay_ms(1000);
}
}
