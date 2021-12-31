/*
 * Author : VAIBHAV.
 * DATE : 31 DECEMBER 2021.
 * PURPOSE : TO MEASURE THE TIME INTERVAL BETWEEN TWO CONSECATIVE PULSE IN MILLI SECONDS USING TIMER-COUNTER.
 */
#define PIN 8  // INPUT CAPTURE PIN
#define TIME_TO_ONE_TICK  64e-6  // (16e16/1024)^-1  Time Required to increment counter by one
#include<avr/interrupt.h>


long unsigned int t = 0;
long unsigned int t_p1 = 0;  // Variable to store a capture timestamp of pulse_1
long unsigned int t_p2 = 0;  // Varible to store a capture timestamp of pulse_2
long unsigned int t_diff = 0; // Varible to store a difference between two timestamp
float t_ms = 0.00; // varible to store a final value

bool pulse_1_occured = false;
bool pulse_2_occured = false;

unsigned int no_of_cycle = 0;


void setup()
{
  cli();
  TCCR1A= 0x00; // Initialize timer1 in normal mode operation free running mode 0x0000 to 0xffff
  //TCCR1B = 0x05; // prescalar to 1024
  TCCR1B = 0x85; // Enable Noice Cancel on ICP1, Falling  EDGE Triggering, Prescalar = 1024
  TCCR1C = 0x00;
  Serial.begin(9600);
  TCNT1 = 0x0000; // initilize timer1 with 0x0000
  TIMSK1 = 0x21;  // Unmask timer1 overflow interrupt and ICP event interrupt
  pinMode(PIN,INPUT_PULLUP);
  pinMode(PIN,INPUT);
  sei();
}

void loop()
{
  if( pulse_1_occured && pulse_2_occured )
    {
      if( no_of_cycle > 0)
        t_diff = ( (0xFFFF - t_p1) + ( 0xFFFF*(no_of_cycle - 1) )  + t_p2  );
      else
        t_diff = (t_p2 - t_p1);

      t_ms = (t_diff * TIME_TO_ONE_TICK)* 1000 ;
      Serial.print("Difference is:");
      Serial.print(t_ms);
      Serial.println(" msec");

      no_of_cycle = 0;
      t_diff = 0;
      t_ms = 0;
      pulse_1_occured = false;
      pulse_2_occured = false;
    }

  
}

ISR(TIMER1_OVF_vect)
{
  no_of_cycle++;
}

ISR(TIMER1_CAPT_vect)
{
  if( ! pulse_1_occured )
   {
    pulse_1_occured = true;
    t_p1 = ICR1;
    no_of_cycle=0;
   }
  else 
   {
    pulse_2_occured = true;
    t_p2 = ICR1;
   }
}
