#include <avr/interrupt.h>
#include <avr/io.h>

uint8_t chan;
uint8_t ctl_pos;

int ch_2_state;
int ch_3_state;

void main() {
// Pin configuratin
  DDRB =0x1B;
  PORTB=0;

// Timer0 Setup
  TCCR0A=0x83;
  TCCR0B=0x02; // 0x05 - 30 Hz, 0x04 - 122 Hz, 0x03 - 488 Hz
  TCNT0 =0;
  OCR0B =0;

// Timer1 Setup
  PLLCSR=0x86;
  TCCR1 =0x8a;
  GTCCR =0x00;
  TCNT1 =0x00;
  OCR1A =183;
  OCR1B =0xff;
  OCR1C =0xff;
  TIMSK =0x40;

// CPU setup
  MCUCR =0x03;
  PRR   =0x03;
  GIMSK =0x40;
  PORTB =0;

// Init section
  chan=0;
  ch_2_state=0;
  ch_3_state=0;

// Start
  sei();
  while(1)
    asm("nop");
}


ISR(TIM1_COMPA_vect) {
  chan=0; // reset channel at start of each frame
}

ISR(INT0_vect) {
  ctl_pos=TCNT1; TCNT1=0; // read timing and reset timer
  switch (chan) {
    case 0:
      PORTB=PORTB & 0xF7 | 8; // Translate channel 1 as-is to servo
      break;
    case 1:
      PORTB=PORTB & 0xF7 | 0; // FIXIT: MCU have bit-manipulating instructions
      break;
    case 3:
      if (ctl_pos<0x88) { // 0x88, 0x48 - min/max hadnle positions
        if (ctl_pos>0x48) {
          OCR0A=(ctl_pos-0x48)<<2;
          TCCR0A=0x83; // Start PWM
        } else {
          TCCR0A=0x03; // Stop PWM
          PORTB=PORTB & 0xFE; // Output 0
        }
      } else {
        TCCR0A=0x03; // Stop PWM
        PORTB=PORTB & 0xFE | 1; // Output 1
      }
      break;
    case 5: // switch positions
      if (ctl_pos>0x76) ch_3_state=0x10;
      if (ctl_pos<0x56) ch_3_state=0;
      PORTB=PORTB & 0xEF | ch_3_state;
      break;
    case 6: // switch positions
      if (ctl_pos>0x76) ch_2_state=2;
      if (ctl_pos<0x56) ch_2_state=0;
      PORTB=PORTB & 0xFD | ch_2_state;
      break;
  }
  chan++;
}
