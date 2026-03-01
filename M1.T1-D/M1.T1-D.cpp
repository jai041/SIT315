#include <avr/io.h>
#include <avr/interrupt.h>


// -------- PIN ASSIGNMENTS --------

const uint8_t PIR_INPUT = 8;
const uint8_t BUTTON_INPUT = 9;
const uint8_t ULTRA_TRIG = 12;
const uint8_t ULTRA_ECHO = 11;
const uint8_t STATUS_LED = 13;


// -------- SHARED FLAGS --------

volatile bool motionFlag = false;
volatile bool buttonFlag = false;
volatile bool periodicFlag = false;


// -------- DISTANCE DATA --------

long echoDuration = 0;
int distanceCM = 0;


// -------- TIMER SETUP --------

void initTimer()
{
cli();

TCCR1A = 0;
TCCR1B = 0;

TCNT1 = 0;

OCR1A = 15624;

TCCR1B |= (1 << WGM12);

TCCR1B |= (1 << CS12) | (1 << CS10);

TIMSK1 |= (1 << OCIE1A);

sei();
}


// -------- PCI SETUP --------

void initPCI()
{

PCICR |= (1 << PCIE0);

PCMSK0 |= (1 << PCINT0);
PCMSK0 |= (1 << PCINT1);

}


// -------- SETUP --------

void setup()
{

Serial.begin(9600);


pinMode(PIR_INPUT, INPUT);

pinMode(BUTTON_INPUT, INPUT_PULLUP);

pinMode(ULTRA_TRIG, OUTPUT);

pinMode(ULTRA_ECHO, INPUT);

pinMode(STATUS_LED, OUTPUT);


initTimer();

initPCI();


Serial.println("System Active");

}


// -------- TIMER INTERRUPT --------

ISR(TIMER1_COMPA_vect)
{

periodicFlag = true;

}


// -------- PIN CHANGE INTERRUPT --------

ISR(PCINT0_vect)
{

if (digitalRead(PIR_INPUT) == HIGH)
{

motionFlag = true;

}


if (digitalRead(BUTTON_INPUT) == LOW)
{

buttonFlag = true;

}

}


// -------- DISTANCE FUNCTION --------

void calculateDistance()
{

digitalWrite(ULTRA_TRIG, LOW);

delayMicroseconds(4);

digitalWrite(ULTRA_TRIG, HIGH);

delayMicroseconds(10);

digitalWrite(ULTRA_TRIG, LOW);


echoDuration = pulseIn(ULTRA_ECHO, HIGH);

distanceCM = echoDuration * 0.034 / 2;

}


// -------- MAIN LOOP --------

void loop()
{


// timer-based periodic task

if (periodicFlag)
{

calculateDistance();


digitalWrite(STATUS_LED, !digitalRead(STATUS_LED));


Serial.print("Measured Distance = ");

Serial.print(distanceCM);

Serial.println(" cm");


periodicFlag = false;

}


// motion detection event

if (motionFlag)
{

Serial.println("Interrupt Event: PIR Motion");


motionFlag = false;

}


// button event

if (buttonFlag)
{

Serial.println("Interrupt Event: Button Press");


buttonFlag = false;

}

}