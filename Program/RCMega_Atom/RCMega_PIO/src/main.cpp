#include <Arduino.h>
#include <Servo.h>

#define NODE_MCU_PIN_D5 14
#define NODE_MCU_PIN_D6 12

int WAIT = 200;
float PWR = 0.9;

void SetMotorSpeed(float val) // val = [0,1]
{
  if(val == 0)
  {
    analogWrite(NODE_MCU_PIN_D6, 0);
    analogWrite(NODE_MCU_PIN_D5, 0);
    return;
  }

  bool forward = val > 0;
  val = min((float)fabs(val), 1.0f);
  int intVal = (int)(val * PWMRANGE);

  if(forward)
  {
    analogWrite(NODE_MCU_PIN_D6, 0);
    analogWrite(NODE_MCU_PIN_D5, intVal);
  } else
  {

    analogWrite(NODE_MCU_PIN_D5, 0);
    analogWrite(NODE_MCU_PIN_D6, intVal);
  }
}



void setup()
{
  ESP.eraseConfig();

  Serial.begin(115200);
  while(!Serial);
  Serial.printf("------------------------------------------\n");
  pinMode(NODE_MCU_PIN_D5, OUTPUT);
  pinMode(NODE_MCU_PIN_D6, OUTPUT);
  SetMotorSpeed(0);

  //analogWriteFreq(1024);
}

long lastTimePulse = 0;
int pulseNo = 0;

double pulseLenMicros = 500;
const long frequency = 50;
const long pauseLenMicros = (1000000 / frequency) - pulseLenMicros; // Calculated

void loop()
{
  digitalWrite(NODE_MCU_PIN_D6, 0);
  digitalWrite(NODE_MCU_PIN_D5, 1);
  delayMicroseconds((long)pulseLenMicros);
  digitalWrite(NODE_MCU_PIN_D6, 1);
  digitalWrite(NODE_MCU_PIN_D5, 0);
  delayMicroseconds((long)pulseLenMicros);

if(Serial.available())
{
  int w = Serial.read();
  if(w == 'q')
  {
    pulseLenMicros = pulseLenMicros * 1.2;
    Serial.printf("Q\n");
  }
  if(w == 'a')
  {
    pulseLenMicros = pulseLenMicros / 1.2;
    Serial.printf("A\n");
  }
}

/*

  if(((millis() / 20) % 2) == 0)
  {
    PWR = 0;
  } else
    PWR = 1;

  //PWR = fabs(cosf(millis() / 1000.0))*0.5;
  if(micros() - lastTimePulse > WAIT)
  {
    lastTimePulse = micros();

      SetMotorSpeed(PWR);

    pulseNo++;
  }*/
}
