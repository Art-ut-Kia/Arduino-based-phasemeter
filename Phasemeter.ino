/************************************************************************************
 * Phasemeter.ino :                                                                 *
 * - Receives negative pulses of 1ms at a rate of 50Hz on both inputs #2 and #3 :   *
 *                           <1><--------19-------> ms                              *
 *                   ________   ________________   _____________________            *
 *            #2             |_|                |_|                                 *
 *                   ______________________   ________________   _______            *
 *            #3                           |_|                |_|                   *
 *                            <---- d ---->                                         *
 * - Interrupt handlers are attached to these pins, both sensitive to falling edges *
 * - At each falling edge on input #3, transmits the time "d" elapsed since last    *
 *   falling edge on pin #2.                                                        *
 ************************************************************************************/
//#define autotest
 
volatile long int t0;

void setup() {
  // shut-down LED on pin #13
  pinMode(13, OUTPUT); digitalWrite(13, LOW);

# ifdef autotest
  // for testing purpose : pins #4 & #5 are used to stimulate pins #2 and #3
  pinMode( 4, OUTPUT); digitalWrite( 4, HIGH);
  pinMode( 5, OUTPUT); digitalWrite( 5, HIGH);
# endif

  // utility code : initializes the phase-meter
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), int2, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), int3, FALLING);
}

void loop() {
# ifdef autotest
  // sends a 1ms negative pulse on output #4
  digitalWrite(4, LOW);
  delay(1);
  digitalWrite(4, HIGH);
  // wait for the desired test duration minus 1ms
  char DelayMillis = 3;
  delay(DelayMillis - 1);
  // sends a 1ms negative pulse on output #5
  digitalWrite(5, LOW);
  delay(1);
  digitalWrite(5, HIGH);
  // waits for the balance for 20ms
  delay(20 - 2 - DelayMillis);
# endif
}

void int2() {
  t0 = micros();
}

void int3() {
  // ASCII string terminated by CR/LF (for direct reading in µs on hyperteerminal)
//Serial.println(micros()-t0);
  // 1-character binary, [0 ... 255] domain, LSB = 128µs (0 .. 32.64ms)
  Serial.write(constrain((micros()-t0)>>7, 0, 255));
}
