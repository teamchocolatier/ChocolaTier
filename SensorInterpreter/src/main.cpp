#include <Arduino.h>
#include "DHT.h"

#define ANALOGOUT 3
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

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
  pinMode(A0, INPUT);
  Serial.begin(9600);
  pinMode(ANALOGOUT, OUTPUT);

  dht.begin();
}

void loop() {
    int val1 = analogRead(A0);
    double temp1 = getTemp(val1);

    float chamberT = dht.readTemperature();
    //Serial.print(val1);

    int output = ((70-temp1)*255/70.0);
    Serial.print(temp1);
    Serial.print(',');
    Serial.println(chamberT);
    //Serial.println(output);

    analogWrite(ANALOGOUT, output);

    delay(250);
}
