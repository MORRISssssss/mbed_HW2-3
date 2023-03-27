#include "mbed.h"

PwmOut LED(D6);
AnalogIn lightSensor(A0);
AnalogOut  aout(PA_4);
InterruptIn btn(BUTTON1);
DigitalOut led(LED1);

Thread LEDThread; 
Thread sensorThread;

float sample[20];
float sensorValue[1000];
bool sending = false;

void pwmLED()
{
    int cnt = 0;
    while (true)
    {
        LED.pulsewidth_us(5000 * sample[cnt]);
        cnt++;
        if (cnt == 20) cnt = 0;
        ThisThread::sleep_for(50ms);
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
    for (int i = 0; i < 20; i++){
        if (i < 5)
            sample[i] = (float)i / 5;
        else if (i < 10)
            sample[i] = 1.;
        else if (i < 15)
            sample[i] = 1. - (i - 10.) / 5;
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