#include <Arduino.h>
#include "DHT.h"

#define ANALOGOUT 3
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

#define THERM1 A0
#define THERM2 A2
#define THERM3 A3
#define THERM4 A4
#define THERM5 A5

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor.

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
  pinMode(THERM5, INPUT);
  Serial.begin(9600);
  pinMode(ANALOGOUT, OUTPUT);

  dht.begin();
}

void loop() {
    double temp1 = getTemp(analogRead(THERM1));
    double temp2 = getTemp(analogRead(THERM2));
    double temp3 = getTemp(analogRead(THERM3));
    double temp4 = getTemp(analogRead(THERM4));
    double temp5 = getTemp(analogRead(THERM5));

    float chamberT = dht.readTemperature();
    //Serial.print(val1);

    int output = ((70-temp1)*255/70.0);
    Serial.print(temp1);
    Serial.print(',');
    Serial.print(temp2);
    Serial.print(',');
    Serial.print(temp3);
    Serial.print(',');
    Serial.print(temp4);
    Serial.print(',');
    Serial.println(temp5);
    //Serial.println(chamberT);
    //Serial.println(output);

    analogWrite(ANALOGOUT, output);

    delay(250);
}
