#include <Arduino.h>

// OUTPUT PINS
#define ANALOGOUT 3
#define ANALOGOUT2 10

// THERMISTOR PINS
#define THERM1 A0
#define THERM2 A2
#define THERM3 A3
#define THERM4 A4
#define THERM5 A5

// CUTOFF FREQ FOR LP (in Hz)
#define FC     0.25

// THERMISTOR LOOKUP TABLE {READ IN VAL, TEMP(C)}
double therm[][2] = {{0, 400.0},
{2, 275.0},
{11, 175.0},
{21, 145.0},
{31, 130.0},
{40, 120.0},
{60, 105.0},
{93, 90.0},
{145, 75.0},
{305, 50.0},
{351, 45.0},
{402, 40.0},
{515, 30.0},
{575, 25.0},
{635, 20.0},
{693, 15.0},
{749, 10.0},
{799, 5.0},
{845, 0.0},
{1024, -50.0}};

// HELPER FUNCTION: input analogRead value, outputs temperature in Celcius
double getTemp(double valIn) {
  int i;
  for (i = 0; i < 19; i++) {
    if ((valIn >= therm[i][0]) && (valIn < therm[i+1][0])) {
      double temp = therm[i][1] + (therm[i+1][1] - therm[i][1])/(therm[i+1][0] -
                    therm[i][0]) * (valIn - therm[i][0]);
      return temp;

    }
  }
  return -255;
}

// ----------------------------------------------------------------------------

void setup() {
  Serial.begin(9600); // for debugging

  // set input pins
  pinMode(THERM1, INPUT);
  pinMode(THERM2, INPUT);
  pinMode(THERM3, INPUT);
  pinMode(THERM4, INPUT);
  pinMode(THERM5, INPUT);

  // set output pins
  pinMode(ANALOGOUT, OUTPUT);
  pinMode(ANALOGOUT2, OUTPUT);
}

void loop() {
    double dt = 0.250;  // loop time in seconds
    static unsigned long millisLast = 0; // elapsed time at end of last call to loop
    static double aveT1 = 22; ; // EXTRUDER TEMP : initialize to room temp
    static double aveT2 = 22; ; // NOZZLE TEMP   : initialize to room temp

    // READ TEMPERATURES
    double temp1 = getTemp(analogRead(THERM1));
    double temp2 = getTemp(analogRead(THERM2));
    double temp3 = getTemp(analogRead(THERM3));
    double temp4 = getTemp(analogRead(THERM4));
    double temp5 = getTemp(analogRead(THERM5));

    // LOW PASS + AVERAGE
    double beta = (6.283185*dt*FC)/(6.283185*dt*FC+1);
    // aveT1 = (1-beta)*aveT1 + beta*(temp1+temp2+temp3)/3.0;  //LP-average
    aveT1 = (1-beta)*aveT1 + beta*(temp1+temp2+temp3)/3.0;  //LP-average using only THERM1
    aveT2 = (1-beta)*aveT2 + beta*(temp4+temp5)/2.0;  //LP-average

    // OUTPUT (using a sort of jank fix)
    double sendT1 = (aveT1-1.47)/0.982; // Imaginary temperature that compensates for circuit offset
    double sendT2 = (aveT2-1.47)/0.982; // Imaginary temperature that compensates for circuit offset
    double output1 = constrain((70.0-sendT1)*255.0/70.0, 0, 255);
    double output2 = constrain((70.0-sendT2)*255.0/70.0, 0, 255);
    analogWrite(ANALOGOUT, (int) output1);
    analogWrite(ANALOGOUT2, (int) output2);

    // SERIAL OUT (FOR DEBUGGING)
    // Serial.println(aveT1);

    Serial.print(temp1);
    Serial.print(',');
    Serial.print(temp2);
    Serial.print(',');
    Serial.print(temp3);
    Serial.print(',');
    Serial.print(temp4);
    Serial.print(',');
    Serial.print(temp5);
    Serial.print(", aveT1 = ");
    Serial.print(aveT1);
    Serial.print(", aveT2 = ");
    Serial.println(aveT2);

    while (millis() < millisLast + dt*1000); // wait until dt has passed
    millisLast = millis();
}
