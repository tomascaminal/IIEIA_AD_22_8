/*
Autors: 			Marcel Monereo, Javier Bustos, 
							Alexander Jadnov, Tomàs Caminal
Data: 				24/12/2022
Assignatura:	IIEIA
*/

#include <reg51.h>
sbit AdcWr = P3^0;
sbit AdcIntr = P3^2;

#define VRef 5


#define LightI P0
#define LEDs P2
//Variables declaration
bit Digit = 0x20^0;
unsigned char Tens;
unsigned char Ones;
unsigned char Count2;

float PercentualLightI;

unsigned char FlagT1;
unsigned char FlagEOC;

//Functions declaration
void initialize(void);
void dynVisualize(void);
float CalcLightI(unsigned char AnalogData);
void CalcDigits(float LightInput);
void RSI_T0(void);
void RSI_T1(void);
void RSI_INT0(void);

void main(void)
{
	initialize();
	while(1)
	{
		if (FlagT1==1)
		{
			AdcWr = 0;
			AdcWr = 1;		//Order of initializaing the conversion
			FlagT1 = 0;
			while(FlagEOC==0);
			PercentualLightI = CalcLightI(LightI);
			CalcDigits(PercentualLightI);
			FlagEOC = 0;
		}
	}
	
}

void initialize(void)
{
	LEDs = 0x00;	//Turn off the leds
	AdcIntr = 1;
	//Configure the timer T0 & T1
	TMOD = 0x11;	//Mode 1 selected
	TH0 = 0xFA;		//FFFF-1388=FA93 5ms 0xFA
	TL0 = 0x93;		//									 0x93
	TH1 = 0x3C;		//FFFF-C350=3CAF 5ms 0x3C
	TL1 = 0xAF;		//									 0xAF
	IE = 0x8F;		// Enable interruption  T0, T1, INT0, INT1
	IP = 0x05; 		// Priority is given to external interruptions
	//External interruptions triggered by descendent flank
	IT0 = 1;
	IT1 = 1;
	//Start Counters
	TR0 = 1;
	TR1 = 1;
}

float CalcLightI(unsigned char AnalogData)
{
	float PercentualI;
	LightI = (AnalogData/255)*100;
	return PercentualI;
}

void CalcDigits(float LightInput)
{
	if (LightInput < 100 && LightInput >= 10)
	{
		Tens = LightInput/10;
		Ones = (LightInput-Tens*10);
	}
	else if (LightInput < 10 && LightInput > 0)
	{
		Tens = 0;
		Ones = LightInput;
	}
	else if (LightInput >= 100)
	{
		Tens = 9;
		Ones = 9;
	}
	else if (LightInput <= 0)
	{
		Tens = 0;
		Ones = 0;
	}
}

void dynVisualize(void)
{
	//Each 5ms the displayed digit changes
	//When Digit = 0 the display on P1.6 works
	//When Digit = 1 the display on P1.7 works
	LEDs = (0x1 << (~Digit*5|Digit*6))|(~Digit*Tens|Digit*Ones);
}

void RSI_T0 (void) interrupt 1
{
	TH0 = 0xFA;
	TL0 = 0x93;
	dynVisualize();
	Digit = ~Digit;
}

void RSI_T1 (void) interrupt 3
{
	TH1 = 0x3C;	//FFFF-C350=3CAF 50ms 0x3C 
	TL1 = 0xAF;	//0xAF
	Count2++;
	if (Count2==2) //50ms*2=100ms
	{
		Count2 = 0;
		FlagT1 = 1;
	}
}

void RSI_INT0 (void) interrupt 0
{
	FlagEOC = 1;
}