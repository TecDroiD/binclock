/*
 * binclock.c
 *
 *  Created on: 26.08.2013
 *      Author: tecdroid
 */

#include <avr/io.h>
#include <avr/delay.h>

#include <stdint.h>


/* set this switch if your clock is battery powered
 * it will switch off the lights after 10 seconds
 */
#define WITH_ENLIGHT

#define STATE_NONE			0x00
#define STATE_MENU_SECOND	0x10
#define STATE_MENU_MINUTE	0x20
#define STATE_MENU_HOUR		0x30
#define STATE_MENU_END		0x40

#define DDR_HOUR			DDRD
#define PORT_HOUR			PORTD
#define PORTMASK_HOUR		0b11110000
#define DDR_MINUTE			DDRC
#define	PORT_MINUTE			PORTC
#define PORTMASK_MINUTE		0b00111111
#define DDR_SECOND			DDRB
#define PORT_SECOND			PORTB
#define PORTMASK_SECOND		0b00111111

#define DDR_INPUTS			DDRD
#define PIN_INPUTS			PIND
#define PORT_INPUTS			PORTD
#define	PIN_A				(1<<PIND2)
#define PIN_B				(1<<PIND3)

typedef struct {
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
} s_time;

uint8_t state;
uint8_t enlight;
s_time time;

void inline add_hour(s_time *time);
void inline add_minute(s_time *time, uint8_t overflow);
void inline add_second(s_time *time, uint8_t overflow);

uint8_t a_pressed(void);
uint8_t b_pressed(void);

void display_seconds(s_time time);
void display_minutes(s_time time);
void display_hours(s_time time);
void set_all_leds (uint8_t set);

void init_timer();
void wait_a_second();

void run_states(uint8_t state, s_time* time);

/**
 * main function
 */
int main(int argc, char **argv) {
	// initialisiere alle Werte
	state = STATE_NONE;
	enlight = 10;

	time.hour = 1;
	time.minute = 0;
	time.second = 0;

	// Ausgänge für Stundenzeiger
	DDR_HOUR |= PORTMASK_HOUR;
	// Ausgänge für Minutenzeiger
	DDR_MINUTE |= PORTMASK_MINUTE;
	// Ausgänge für Sekundenzeiger
	DDR_SECOND |= PORTMASK_SECOND;

	// Eingänge für Menütaster, aktiviere Pullups
	DDRD &= ~(1 << PORTD2 | 1 << PORTD3);
	PORTD |= (1 << PORTD2 | 1 << PORTD3);

	// timer initialisieren
	init_timer();

	while (1) {

		// State machine zum Ändern der Uhrzeit
		run_states(state, &time);

		wait_a_second();

		// Prüfe ob Menuetaste gedrückt wurde und ändere Status
		if (a_pressed()) {

			state += 0x10;
			if ((state & 0xf0) == STATE_MENU_END) {
				state = STATE_NONE;
			}
		}

#ifdef WITH_ENLIGHT
		// schalte die Uhrzeitanzeige an.
		if (b_pressed()) {
			enlight = 10;
		}
#endif


	}
}

/**
 * State machine for menu and display
 */
void run_states(uint8_t state, s_time* time) {
	// State machine zum Ändern der Uhrzeit
	switch (0xf0 & state) {
	case STATE_MENU_SECOND:
		set_all_leds(1);
		if (b_pressed()) {
			add_second(time, 0);
		}
		display_seconds(*time);
		break;
	case STATE_MENU_MINUTE:
		set_all_leds(1);
		if (b_pressed()) {
			add_minute(time, 0);
		}
		display_minutes(*time);
		break;
	case STATE_MENU_HOUR:
		set_all_leds(1);
		if (b_pressed()) {
			add_hour(time);
		}
		display_hours(*time);
		break;
	case STATE_NONE:

		set_all_leds(0);

#ifdef WITH_ENLIGHT
		if (enlight > 0) {
#endif
			display_seconds(*time);
			display_minutes(*time);
			display_hours(*time);
#ifdef WITH_ENLIGHT
			enlight--;
		}
#endif

		add_second(time, 1);
		break;
	}
}

/**
 * add one to the hours
 */
void inline add_hour(s_time *time) {
	time->hour++;
	if (time->hour > 12) {
		time->hour = 1;
	}
}

/**
 * add one to the minutes
 */
void inline add_minute(s_time *time, uint8_t overflow) {
	time->minute++;
	if (time->minute >= 60) {
		time->minute = 0;
		if (overflow) {
			add_hour(time);
		}
	}
}

/**
 * add one to the seconds
 */
void inline add_second(s_time *time, uint8_t overflow) {
	time->second++;
	if (time->second >= 60) {
		time->second = 0;
		if (overflow) {
			add_minute(time, 1);
		}
	}
}

/**
 * return true if a has been set
 */
uint8_t a_pressed(void) {
	return !(PIND & (1 << PIND2));
}

/**
 * return true if b has been set
 */
uint8_t b_pressed(void) {
	return !(PIND & (1 << PIND3));
}

/**
 * display the seconds of time
 */
void display_seconds(s_time time) {
	PORT_SECOND &= ~(PORTMASK_SECOND);
	PORT_SECOND |= (PORTMASK_SECOND & time.second);
}

/**
 * display minutes of time
 */
void display_minutes(s_time time) {
	PORT_MINUTE &= ~(PORTMASK_MINUTE);
	PORT_MINUTE |= (PORTMASK_MINUTE & time.minute);
}

/**
 * display hours of time
 */
void display_hours(s_time time) {
	PORT_HOUR &= ~(PORTMASK_HOUR);

	PORT_HOUR |= (PORTMASK_HOUR & (hour << 4));
}


/**
 * wait a second (or less if in test mode)
 */
void wait_a_second() {
	// zwei mal auf den timer overflow warten (entspricht einer Sekunde)
	TIFR |= (1 << TOV0);
	while (!(TIFR & (1 << TOV0)))
		;
	TIFR |= (1 << TOV0);
	while (!(TIFR & (1 << TOV0)))
		;
}

void set_all_leds (uint8_t set) {
	if (set) {
		PORT_SECOND |= (PORTMASK_SECOND);
		PORT_MINUTE |= (PORTMASK_MINUTE);
		PORT_HOUR   |= (PORTMASK_HOUR);
	} else {
		PORT_SECOND &= ~(PORTMASK_SECOND);
		PORT_MINUTE &= ~(PORTMASK_MINUTE);
		PORT_HOUR   &= ~(PORTMASK_HOUR);

	}
}

/**
 * initialize timer
 */
void init_timer() {
	// Timer initialisieren F_CPU/64
	TCCR0 |= ((1 << CS01) | (1 << CS00));
}
