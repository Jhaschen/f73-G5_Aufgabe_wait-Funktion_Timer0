#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "f73-rncontrol-lib/uart.h"
#include "f73-rncontrol-lib/adc.h"
#include "f73-rncontrol-lib/button.h"
#include "f73-rncontrol-lib/led.h"
#include "f73-rncontrol-lib/counter.h"

static void WaitTimer0_x_10ms(uint8_t x_10ms)
{
	// init Timer0
	// Zählerstandsregister zurücksetzen
	counter0SetValue(0);

	// Vergleichregister zurücksetzen
	counter0SetCompare(0);

	// Konfigurationsregister
	//  WGM1:0 = normaler Betrieb
	//  COM1:0 = normaler Betrieb
	//  CS02:0 = Vorteiler 1024
	counter0Start(ATMEGA32_COUNTER_0_PRESCALER_1024);

	// Überlaufzähler
	uint8_t OVcnt = 0;
	// Wir warten x Überlaufe des Timer0 ab.
	// 1 Überlauf => 9.984ms
	
	for (OVcnt = 0; OVcnt < x_10ms; OVcnt++)
	{
		// Vorladen des Zählerregisters, so dass der Überlauf nach 256-100= 156 Takten
		//  auftritt. 156x64us = 9.984ms (ca. 10 ms)
		counter0SetValue(100);

		// Durch Abfragen des Bits TOV0 im Register TIFR können wir testen, ob
		// ein Überlauf des Timer0 erfolgt ist.
		// TOV0=0 kein Überlauf / TOV0=1 Überlauf
		while (counter0HasOverflow() == false)
		{
		}
		// An dieser Stelle ist ein Überlauf augetreten, das Bit TOV0 ist gesetzt. Für den
		//  nächsten Überlauf muss das Bit wieder auf "0" zurückgesetzt werden.
		//  Dieses geschieht in dem eine "1" in das Bit geschrieben wird.
		// Klingt komisch, macht Atmel aber so...
		counter0ClearOverflow();
	}
}

static void WaitTimer0_x_ms(uint16_t x_ms)
{
	// init Timer0
	// Zählerstandsregister zurücksetzen
	counter0SetValue(0);

	// Vergleichregister zurücksetzen
	counter0SetCompare(0);

	// Konfigurationsregister
	//  WGM1:0 = normaler Betrieb
	//  COM1:0 = normaler Betrieb
	//  CS02:0 = Vorteiler 64
	counter0Start(ATMEGA32_COUNTER_0_PRESCALER_64);

	// Überlaufzähler
	uint16_t OVcnt = 0;
	// Wir warten x Überlaufe des Timer0 ab.
	// 1 Zählschritt tSTEP => 64x62.5ns=4us
	// 1 Überlauf => 256x4us=1.024ms => 1024us
	// tOVF = 1ms

	for (OVcnt = 0; OVcnt < x_ms; OVcnt++)
	{
		// Vorladen des Zählerregisters
		// TCNT Prelod = 256 - tOVF/tSTEP = 1ms/4us = 1 x10x-3s / 4x10-6s = 250
		// 250x4us = 1ms 
		counter0SetValue((256-250));

		// Durch Abfragen des Bits TOV0 im Register TIFR können wir testen, ob
		// ein Überlauf des Timer0 erfolgt ist.
		// TOV0=0 kein Überlauf / TOV0=1 Überlauf
		while (counter0HasOverflow() == false)
		{
		}
		// An dieser Stelle ist ein Überlauf augetreten, das Bit TOV0 ist gesetzt. Für den
		//  nächsten Überlauf muss das Bit wieder auf "0" zurückgesetzt werden.
		//  Dieses geschieht in dem eine "1" in das Bit geschrieben wird.
		// Klingt komisch, macht Atmel aber so...
		counter0ClearOverflow();
	}
}

int main()
{
	// init LED
	ledInit();

	while (1)
	{

		// Wait-Funktion Timer0 50x10ms= 50ms =>10Hz 
		// Pro Sekunde 10x ein- und 10x ausgeschaltet
		WaitTimer0_x_ms(50);
		// Wait-Funktion Timer0 25x10ms= 250ms => 2Hz
		//WaitTimer0_x_10ms(25);
		// Jetzt wird die LED getoggelt !
		ledToggle(8);
	}
	return 0;
}
