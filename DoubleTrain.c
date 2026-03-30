#include <avr/io.h>

// =====================
// Définition des sorties et entrées
// =====================

// Train 1
#define M1 0x08  // PA3
#define A1 0x04  // PA2
#define B1 0x02  // PA1
#define C1 0x01  // PA0
#define PA7_T1 0x80 // PA7 pour bypass si nécessaire (train 1)

// Train 2
#define M2 0x80  // PA7
#define A2 0x40  // PA6
#define B2 0x20  // PA5
#define C2 0x10  // PA4

// Sorties train 1
#define ON1 0x01   // PB0
#define AR1 0x02   // PB1

// Sorties train 2
#define ON2 0x20   // PB6
#define AR2 0x10   // PB7

// =====================
// Etats train 1
// =====================
typedef enum {
	ARRET_INIT = 0,
	ADV_B1,
	REVERS_A1,
	ADV_C1,
	REVERS_STOP
} State1;

// Etats train 2
typedef enum {
	ARRET2_INIT = 0,
	ADV2,
	REV2,
	REV2_STOP
} State2;

// =====================
// Fonction de gestion train 1
// =====================
unsigned char gere_train1(unsigned char inputByte)
{
	static State1 state = ARRET_INIT;
	unsigned char outputByte = 0;

	switch(state)
	{
		case ARRET_INIT:
		outputByte = 0; // Stop
		if(inputByte & M1)
		state = ADV_B1;
		break;

		case ADV_B1:
		outputByte = ON1;
		if(inputByte & PA7_T1) // bypass
		state = ADV_C1;
		else if(inputByte & B1)
		state = REVERS_A1;
		break;

		case REVERS_A1:
		outputByte = ON1 | AR1;
		if(inputByte & A1)
		state = ADV_C1;
		break;

		case ADV_C1:
		outputByte = ON1;
		if(inputByte & C1)
		state = REVERS_STOP;
		break;

		case REVERS_STOP:
		outputByte = ON1 | AR1;
		if(inputByte & A1)
		state = ARRET_INIT;
		break;

		default:
		state = ARRET_INIT;
		outputByte = 0;
		break;
	}

	return outputByte;
}

// =====================
// Fonction de gestion train 2
// =====================
unsigned char gere_train2(unsigned char inputByte)
{
	static State2 state2 = ARRET2_INIT;
	unsigned char outputByte2 = 0;

	switch(state2)
	{
		case ARRET2_INIT:
		outputByte2 = 0; // Stop
		if(inputByte & M2)
		state2 = ADV2;
		break;


		case ADV2:
		outputByte2 = ON2;
		if(inputByte & C2)
		state2 = REV2;
		break;

		case REV2:
		outputByte2 = ON2 | AR2;
		if(inputByte & B2)
		state2 = ADV2;
		if(inputByte & A2)
		state2 = ARRET2_INIT;
		break;

		default:
		state2 = ARRET2_INIT;
		outputByte2 = 0;
		break;
	}

	return outputByte2;
}

// =====================
// MAIN
// =====================
int main(void)
{
	unsigned char inputByte;
	unsigned char outputTrain1;
	unsigned char outputTrain2;

	// Config sorties
	DDRB = ON1 | AR1 | ON2 | AR2; // PB0, PB1, PB6, PB7 sorties
	DDRA = 0x00; // PA0..PA7 entrées

	while(1)
	{
		// Lecture entrée UNE seule fois
		inputByte = PINA;

		// Gestion trains
		outputTrain1 = gere_train1(inputByte);
		outputTrain2 = gere_train2(inputByte);

		// Écriture sortie combinée
		PORTB = outputTrain1 | outputTrain2;
	}
}
