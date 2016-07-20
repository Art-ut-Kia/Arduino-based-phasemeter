//---------------------------------------------------------------------------------------//
// Phasemeter.ino :                                                                      //
// - Receives two negative pulses of 1ms at a rate of 50Hz on inputs #2 and #3 :         //
//                           <1><--------19----> (ms)                                    //
//                   ________   ________________   _____________________                 //
//            #2             |_|                |_|                                      //
//                   ______________________   ________________   _______                 //
//            #3                           |_|                |_|                        //
//                            <---- d ---->                                              //
// - Interrupt handlers are attached to these pins, both sensitive to falling edges      //
// - At each falling edge on input #3, transmits on serial link and on an LCD the        //
//   time "d" elapsed from the last falling edge on pin #2.                              //
//                         Author : Jean-Paul PETILLON                                   //
//------------------------------------ GNU GPL ------------------------------------------//
// This program is free software: you can redistribute it and/or modify it under the     //
// terms of the GNU General Public License as published by the Free Software Foundation, //
// either version 3 of the License, or (at your option) any later version.               //
//                                                                                       //
// This program is distributed in the hope that it will be useful, but without any       //
// warranty; without even the implied warranty of merchantability or fitness for a       //
// particular purpose. See the GNU General Public License for more details.              //
//                                                                                       //
// You should have received a copy of the GNU General Public License along with this     //
// program. If not, see <http://www.gnu.org/licenses/>.                                  //
//---------------------------------------------------------------------------------------//

#include <LiquidCrystal.h>

#define uselcd   // comment or uncomment the line to implement the function
//#define selftest // comment or uncomment the line to implement the function

#ifdef uselcd  // RS, EN, D4, D5, D6, D7
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
#endif // (def selftest)

volatile long int t0;
volatile unsigned char phase;

void setup() {
  // shutdown the LED connected to pin #13
  pinMode(13, OUTPUT); digitalWrite(13, LOW);

# ifdef selftest
  // for testing purpose: pins #4 & #5 are used to stimulate pins #2 and #3
  pinMode(4, OUTPUT); digitalWrite(4, HIGH);
  pinMode(5, OUTPUT); digitalWrite(5, HIGH);
# endif // (def selftest)

  // initializes the phase-meter
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), int2, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), int3, FALLING);

  // initializes the LCD
# ifdef uselcd
  lcd.begin(16, 2);    // defines the LCD type
  lcd.setCursor(0, 0); // set the cursor at the begining of the 1st line
  lcd.print(" phase | Phase:");
  lcd.setCursor(0, 1); // set the cursor at the begining of the 2nd line
  lcd.print(" meter |      ms");
# endif // (def selftest)
}

void loop() {
  //-------------------- generates two test signals, on pins #4 & #5 ----------------------
# ifdef selftest
  // sends a 1ms negative pulse on output #4
  digitalWrite(4, LOW);
  delay(1);
  digitalWrite(4, HIGH);
  // waits for the desired test duration minus 1ms
  char DelayMillis = 3;
  delay(DelayMillis - 1);
  // sends a 1ms negative pulse on output #5
  digitalWrite(5, LOW);
  delay(1);
  digitalWrite(5, HIGH);
  // waits for the balance to 20ms (50Hz)
  delay(20 - 2 - DelayMillis);
# endif // (def selftest)

  //--------------------- displays the measurement on the 2 x 16 LCD ----------------------
# ifdef uselcd
  float fPhase = 0.128 * phase; // recomputes measurement in millisecond, floating point
  lcd.setCursor(9, 1); // cursor at the begining of the phase display field
  if (fPhase<10) lcd.print(" "); // to deal with the variable length of the phase display
  lcd.print(fPhase, 1);
  lcd.setCursor(13,1);
  lcd.print(" ");
# endif // (def uselcd)
  //------------------------------------ end of loop() ------------------------------------
}

void int2() {
  t0 = micros(); // memorizes the date in µs at falling edge on pin #2
}

void int3() {
  // computes delay (in 128µs quanta) between falling edges on pin #2 and pin #3:
  //                               micros()-t0
  phase = constrain((micros()-t0)>>7, 0, 255);

  // transmits it over the Arduino UART Tx signal
  // ASCII string (in ms) terminated by CR/LF (for direct reading in hyperterminal)
//Serial.println(0.128 * phase);
  // single character binary, [0 ... 255] domain, LSB = 128µs (0 .. 32.64ms)
  Serial.write(phase);
}
