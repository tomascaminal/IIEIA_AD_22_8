/*
Autors: 			Marcel Monereo, Javier Bustos, 
							Alexander Jadnov, Tomàs Caminal
Data: 				24/12/2022
Assignatura:	IIEIA
*/

#include <reg51.h>
sbit WR = P3^0;
sbit INTR = P3^1;

#define LEDs = P2;
//Declaració de variables


void main(void)
{
	P1=0x00;
	P2=0x00;
	P3=0x00;
}