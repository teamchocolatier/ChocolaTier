#include <Arduino.h>

// Temperature goal for extruder
#define GOALTEMP1 32
#define GOALTEMP2 32

#define E1ENABLE true
#define E2ENABLE false

// PID VALUES
#define KP   75
#define KI   1
#define KD   0

#define INTMAX 100

//Heating PWM Pins
#define PWM1 3
#define PWM2 5

//Thermistor read pins
#define THERM1 A0
#define THERM2 A1
#define THERM3 A3
#define THERM4 A4


//{READ IN VAL, TEMP(C)}
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
  pinMode(THERM1, INPUT);
  pinMode(THERM2, INPUT);
  pinMode(THERM3, INPUT);
  pinMode(THERM4, INPUT);
  Serial.begin(9600);
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);

  analogWrite(PWM1, 0);
  analogWrite(PWM2, 0);

}

void loop() {
    double dt = 0.250;  // loop time in seconds
    static unsigned long millisLast = 0; // elapsed time at end of last call to loop
    static double aveT1 = 22; ; // initialize to room temp
    static double aveT2 = 22; ; // initialize to room temp
    static double intTerm1 = 0;
    static double intTerm2 = 0;

    // READ TEMPERATURES
    double temp1 = getTemp(analogRead(THERM1));
    double temp2 = getTemp(analogRead(THERM2));
    double temp3 = getTemp(analogRead(THERM3));
    double temp4 = getTemp(analogRead(THERM4));

    // Low Pass
    double beta = 0.1;
    aveT1 = (1-beta)*aveT1 + beta*(temp1+temp2)/2.0;  //LP-average
    aveT2 = (1-beta)*aveT2 + beta*(temp3+temp4)/2.0;  //LP-average


    // Control Law
    double error1 = GOALTEMP1 - aveT1;
    double error2 = GOALTEMP2 - aveT2;
    static double e1last = error1;
    static double e2last = error2;

    intTerm1 = intTerm1 + KI*error1*dt;
    intTerm2 = intTerm2 + KI*error2*dt;
    if(intTerm1 >  INTMAX) intTerm1 =  INTMAX;
    if(intTerm1 < -INTMAX) intTerm1 = -INTMAX;
    if(intTerm2 >  INTMAX) intTerm2 =  INTMAX;
    if(intTerm2 < -INTMAX) intTerm2 = -INTMAX;

    int output1 = constrain(KP*error1+intTerm1+KD*(error1-e1last)/dt, 0, 255);
    int output2 = constrain(KP*error2+intTerm2+KD*(error2-e2last)/dt, 0, 255);

    // Serial.print(temp1);
    // Serial.print(',');
    // Serial.print(temp2);
    // Serial.print(',');
    // Serial.print(temp3);
    // Serial.print(',');
    // Serial.print(temp4);
    // Serial.print(", aveT = ");
    // Serial.println(aveT1);

    Serial.print(aveT1);
    Serial.print(',');
    Serial.print(error1);
    Serial.print(',');
    Serial.println(output1);

    if(E1ENABLE) analogWrite(PWM1, output1);
    if(E2ENABLE) analogWrite(PWM2, output2);

    while (millis() < millisLast + dt*1000); // wait until dt has passed
    millisLast = millis();
    e1last = error1;
    e2last = error2;
}
