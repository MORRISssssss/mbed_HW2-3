#include "mbed.h"

PwmOut LED(D6);
AnalogIn lightSensor(A0);
AnalogOut  aout(PA_4);
InterruptIn btn(BUTTON1);
DigitalOut led(LED1);

Thread LEDThread; 
Thread sensorThread;

float sample[100];
float sensorValue[1000];
bool sending = false;

void pwmLED()
{
    int cnt = 0;
    while (true)
    {
        LED.pulsewidth_us(5000 * sample[cnt]);
        cnt++;
        if (cnt == 100) cnt = 0;
        ThisThread::sleep_for(10ms);
    }
}

void sensor()
{
    while (true){
        if (sending) {
            for (int i = 0; i < 1000; i++){
                sensorValue[i] = lightSensor.read();
                aout = sensorValue[i];
                ThisThread::sleep_for(1ms);
            }
            sending = !sending;
            led = !led;
        }else {
            aout = lightSensor.read();
            ThisThread::sleep_for(1ms);
        }
    }
}

void sendValue()
{
    sending = !sending;
    led = !led;
}


int main()
{
    for (int i = 0; i < 100; i++){
        if (i < 25)
            sample[i] = (float)i / 25;
        else if (i < 50)
            sample[i] = 1.;
        else if (i < 75)
            sample[i] = 1. - (i - 50.) / 25;
        else
            sample[i] = 0;
    }
    LED.period_ms(5);
    LEDThread.start(pwmLED);
    sensorThread.start(sensor);
    btn.rise(&sendValue);
    while (true){
        if (sending) {
            ThisThread::sleep_for(1500ms);
            for (int i = 0; i < 1000; i++)
                printf("%f\r\n", sensorValue[i]);
        }
        ThisThread::sleep_for(1ms);
    }
}