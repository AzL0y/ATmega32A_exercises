#include <avr/io.h>

// Outputs
#define STOP 0x00
#define ADV  0x01
#define REV  0x03

// Inputs
#define M1 0x08
#define A1 0x04
#define B1 0x02
#define C1 0x01
#define PA7_MASK 0x80

typedef enum {
	ARRET_INIT = 0,
	ADV_B1,
	REVERS_A1,
	ADV_C1,
	REVERS_STOP
} State;

// =========================
// MACHINE D'ÉTAT
// =========================
unsigned char gere_train1(unsigned char inputByte)
{
	static State state = ARRET_INIT;
	unsigned char outputByte;

	switch (state)
	{
		case ARRET_INIT:
		outputByte = STOP;

		if (inputByte & M1)
		state = ADV_B1;
		break;

		case ADV_B1:
		outputByte = ADV;

		if (inputByte & PA7_MASK)
		state = ADV_C1;
		else if (inputByte & B1)
		state = REVERS_A1;
		break;

		case REVERS_A1:
		outputByte = REV;

		if (inputByte & A1)
		state = ADV_C1;
		break;

		case ADV_C1:
		outputByte = ADV;

		if (inputByte & C1)
		state = REVERS_STOP;
		break;

		case REVERS_STOP:
		outputByte = REV;

		if (inputByte & A1)
		state = ARRET_INIT;
		break;

		default:
		state = ARRET_INIT;
		outputByte = STOP;
		break;
	}

	return outputByte;
}

// =========================
// MAIN
// =========================
int main(void)
{
	unsigned char inputByte;
	unsigned char outputByte;

	DDRB = 0x03; // sorties
	DDRA = 0x70; // entrées (PA7 + PA3..PA0)

	while (1)
	{
		// Lecture UNE seule fois
		inputByte = PINA;

		// Appel machine d’état
		outputByte = gere_train1(inputByte);

		// Écriture UNE seule fois
		PORTB = outputByte;
	}
}
