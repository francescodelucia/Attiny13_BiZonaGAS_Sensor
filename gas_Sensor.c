#define F_CPU 9600000 // Define software reference clock for delay 
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 

#define LED_SENSOR1	PB2 

#define ON 	1
#define OFF 	0

#define ADC_SENSOR1 0
#define ADC_SENSOR2 1

#define BIT_L(port,pin) port &= ~(1<<pin)
#define BIT_H(port,pin) port |= (1<<pin)
#define PAUSE _delay_ms(10)

#define SOUND PB0

#define NO_SOUND	BIT_L(PORTB,SOUND);PAUSE
#define PLAY_SOUND 	if(ENABLE_SOUND!=OFF){BIT_H(PORTB,SOUND);PAUSE;}NO_SOUND
#define OFF_LEDS	BIT_L(PORTB,LED_SENSOR1);
#define ACTIONS		BlinkLeds();PlaySound()


volatile int ENABLE_SOUND=OFF;
volatile int POWER=0;

volatile int CURRENT_ADC=0;
volatile int s=0;
volatile int pwm_dcy_led;


void BlinkLeds()
{
	BIT_H(PORTB,LED_SENSOR1);	
	PAUSE;
	PAUSE;
	OFF_LEDS;
}
void PlaySound()
{
	PLAY_SOUND;
}
void SetMUX(int nMUX)
{
	if(nMUX==0)
	{ADMUX = 0b00000010; /*(ADC2)*/}
	else
	{ADMUX = 0b00000011;/*(ADC3)*/	}
}

ISR(PCINT0_vect)      	
{			            
		
	if(s>0)
	{
		ENABLE_SOUND^=(1<<0x01);s=0; 		
		PlaySound();				
		BIT_H(PORTB,SOUND);
		PAUSE;
		NO_SOUND;
		return;
	}
	s++;							
} 

void SystemInit(void) 
{ 
	
	DDRB |= ( 1 << LED_SENSOR1 ) | /*( 1 << LED_SENSOR2 ) | */ ( 1 << SOUND ); 	
	
	ADCSRA |= (1<<ADEN)|  // Analog - Digital  enable  bit
			(1<<ADPS1)|  // set prescalerto 8(clock/8)
			(1<<ADPS0); // set prescaler to 8(clock/8)
	
	PCMSK |= (1<<PCINT1); // pin change mask: listen to portb bit 2 
	GIMSK |= (1<<PCIE);   // enable PCINT interrupt 
	MCUCR = 0b00000010; //(1<<ISC00);
	
	sei();         // enable all interrupts 
} 


int main (void)
{
	SystemInit();	
	int CURRENT_ADC=ADC_SENSOR1;
	
	for (;;)
	{
		SetMUX(CURRENT_ADC);				
		
		ADCSRA |= (1<<ADEN); // Analog Digital enable bit
		ADCSRA |= (1<<ADSC); // Discarte first conversion
		
		while(ADCSRA & (1<<ADSC)) ; // wait until conversion is done
		
		ADCSRA |=(1<<ADSC);  // start single conversion
		
		while(ADCSRA &(1<<ADSC)) // wait until conversion is done
		
		ADCSRA &= ~(1<<ADEN); // shutdown the ADC							
		
		PAUSE;	
		if(ADCH > 1){ACTIONS;}
		if(CURRENT_ADC==ADC_SENSOR1){					
			CURRENT_ADC=ADC_SENSOR2;
		}else{						
			CURRENT_ADC=ADC_SENSOR1;
		}								
	}
	
	return 0;
}
